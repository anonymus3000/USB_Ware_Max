#ifndef USB_TOOLS_H
#define USB_TOOLS_H

#include <map>
#include <vector>
#include <Arduino.h>

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

static USBHIDMouse    Mouse;
static USBHIDKeyboard Keyboard;

static int  defaultDelay   = 30;                     
static int  stringDelay    = 15; // Set to 15ms so text types reliably without garbling
static const long  MAX_DELAY_MS   = 60UL * 1000UL;   
static const int   MAX_CALL_DEPTH = 32;              
static const long  MAX_STEPS      = 2000000L;        

static bool stopRequested  = false;

static std::map<String, String> vars;
static std::map<String, String> defines;

enum Op {
  OP_NOP,
  OP_STRING, OP_STRINGLN, OP_PRINT,
  OP_KEY, OP_COMBO, OP_HOLD, OP_RELEASE, OP_RELEASE_ALL,
  OP_DELAY, OP_DEFAULT_DELAY, OP_STRING_DELAY, OP_WAIT,
  OP_MOUSE_MOVE, OP_MOUSE_CLICK, OP_MOUSE_SCROLL,
  OP_VAR, OP_MATH, OP_IF, OP_ELSE, OP_ENDIF,
  OP_LOOP, OP_ENDLOOP, OP_WHILE, OP_ENDWHILE,
  OP_FUNC, OP_ENDFUNC, OP_CALL, OP_RETURN, OP_REPEAT
};

struct Instr {
  Op     op   = OP_NOP;
  String a; String b; String c;
  long   n    = 0; long m = 0;
  int    j1   = -1; int line = 0;
};

static std::vector<Instr>          program;
static std::map<String, int>       funcStart;

static String clean(const String &in) { String s = in; s.replace("\r", ""); s.trim(); return s; }

static int splitTokens(const String &s, String out[], int maxTok) {
  int count = 0, i = 0, len = s.length();
  while (i < len && count < maxTok) {
    while (i < len && s[i] == ' ') i++;
    if (i >= len) break;
    int start = i;
    while (i < len && s[i] != ' ') i++;
    out[count++] = s.substring(start, i);
  }
  return count;
}

static String expand(const String &in) {
  String out; out.reserve(in.length());
  int i = 0, len = in.length();
  while (i < len) {
    char ch = in[i];
    if (ch == '$' || ch == '#') {
      int j = i + 1; bool brace = false;
      if (ch == '$' && j < len && in[j] == '{') { brace = true; j++; }
      int start = j;
      while (j < len && (isAlphaNumeric(in[j]) || in[j] == '_')) j++;
      String name = in.substring(start, j);
      if (brace && j < len && in[j] == '}') j++;
      std::map<String, String> &tbl = (ch == '$') ? vars : defines;
      if (name.length() && tbl.count(name)) out += tbl[name];
      else                                  out += in.substring(i, j);
      i = j;
    } else {
      out += in[i++];
    }
  }
  return out;
}

struct ExprParser {
  const String &s; int i;
  ExprParser(const String &str) : s(str), i(0) {}
  void skip() { while (i < (int)s.length() && s[i] == ' ') i++; }

  long factor() {
    skip();
    if (i < (int)s.length() && s[i] == '(') { i++; long v = expr(); skip(); if (i < (int)s.length() && s[i] == ')') i++; return v; }
    if (i < (int)s.length() && s[i] == '-') { i++; return -factor(); }
    if (i < (int)s.length() && isDigit(s[i])) {
      int start = i;
      while (i < (int)s.length() && isDigit(s[i])) i++;
      return s.substring(start, i).toInt();
    }
    if (i < (int)s.length() && (s[i] == '$' || isAlpha(s[i]) || s[i] == '_')) {
      if (s[i] == '$') i++;
      int start = i;
      while (i < (int)s.length() && (isAlphaNumeric(s[i]) || s[i] == '_')) i++;
      String name = s.substring(start, i);
      return vars.count(name) ? vars[name].toInt() : 0;
    }
    return 0;
  }
  long term() {
    long v = factor();
    for (;;) { skip();
      if      (i < (int)s.length() && s[i] == '*') { i++; v *= factor(); }
      else if (i < (int)s.length() && s[i] == '/') { i++; long d = factor(); v = d ? v / d : 0; }
      else if (i < (int)s.length() && s[i] == '%') { i++; long d = factor(); v = d ? v % d : 0; }
      else break;
    }
    return v;
  }
  long expr() {
    long v = term();
    for (;;) { skip();
      if      (i < (int)s.length() && s[i] == '+') { i++; v += term(); }
      else if (i < (int)s.length() && s[i] == '-') { i++; v -= term(); }
      else break;
    }
    return v;
  }
};
static long evalExpr(const String &raw) { String e = expand(raw); ExprParser p(e); return p.expr(); }

static bool isNumeric(const String &s) {
  if (!s.length()) return false;
  int i = (s[0] == '-') ? 1 : 0;
  if (i >= (int)s.length()) return false;
  for (; i < (int)s.length(); i++) if (!isDigit(s[i])) return false;
  return true;
}

static String resolveOperand(const String &raw) {
  String v = expand(raw);
  if (v == raw && vars.count(raw)) return vars[raw];
  return v;
}

static bool evalCond(const String &lraw, const String &op, const String &rraw) {
  String left = resolveOperand(lraw), right = resolveOperand(rraw);
  bool num = isNumeric(left) && isNumeric(right);
  long L = left.toInt(), R = right.toInt();
  if (op == "==") return num ? (L == R) : (left == right);
  if (op == "!=") return num ? (L != R) : (left != right);
  if (op == ">")  return L >  R;
  if (op == "<")  return L <  R;
  if (op == ">=") return L >= R;
  if (op == "<=") return L <= R;
  return false;
}

static bool modKey(const String &t, uint8_t &out) {
  String u = t; u.toUpperCase();
  if (u == "CTRL" || u == "CONTROL") { out = KEY_LEFT_CTRL;  return true; }
  if (u == "ALT")                    { out = KEY_LEFT_ALT;   return true; }
  if (u == "SHIFT")                  { out = KEY_LEFT_SHIFT;  return true; }
  if (u == "GUI" || u == "WINDOWS" || u == "WIN" || u == "CMD" || u == "META")
                                     { out = KEY_LEFT_GUI;   return true; }
  return false;
}

static bool namedKey(const String &t, uint8_t &out) {
  String u = t; u.toUpperCase();
  if (u == "ENTER" || u == "RETURN") { out = KEY_RETURN;       return true; }
  if (u == "TAB")                    { out = KEY_TAB;          return true; }
  if (u == "ESC" || u == "ESCAPE")   { out = KEY_ESC;          return true; }
  if (u == "SPACE")                  { out = ' ';              return true; }
  if (u == "BACKSPACE" || u == "BKSP"){ out = KEY_BACKSPACE;   return true; }
  if (u == "DELETE" || u == "DEL")   { out = KEY_DELETE;       return true; }
  if (u == "UP")                     { out = KEY_UP_ARROW;     return true; }
  if (u == "DOWN")                   { out = KEY_DOWN_ARROW;   return true; }
  if (u == "LEFT")                   { out = KEY_LEFT_ARROW;   return true; }
  if (u == "RIGHT")                  { out = KEY_RIGHT_ARROW;  return true; }
  if (u.length() > 1 && u[0] == 'F' && isDigit(u[1])) {
    int f = u.substring(1).toInt();
    if (f >= 1 && f <= 12) { out = KEY_F1 + (f - 1); return true; }
  }
  return false;
}

static bool resolveKey(const String &t, uint8_t &out) {
  if (namedKey(t, out)) return true;
  if (t.length() == 1)  { out = (uint8_t)t[0]; return true; }
  return false;
}

struct Frame { Op op; int idx; };

static void parse(const String &src) {
  program.clear(); funcStart.clear(); defines.clear();
  std::vector<Frame> stk;
  int lineNo = 0, pos = 0, len = src.length();

  while (pos < len) {
    int e = src.indexOf('\n', pos); if (e == -1) e = len;
    String line = clean(src.substring(pos, e)); pos = e + 1; lineNo++;
    if (line.isEmpty() || line.startsWith("REM")) continue;

    int sp = line.indexOf(' ');
    String head = (sp == -1) ? line : line.substring(0, sp);
    String rest = (sp == -1) ? ""   : line.substring(sp + 1);
    String HEAD = head; HEAD.toUpperCase();

    if      (HEAD == "END_IF")       HEAD = "ENDIF";
    else if (HEAD == "END_WHILE")    HEAD = "ENDWHILE";
    else if (HEAD == "END_LOOP")     HEAD = "ENDLOOP";
    else if (HEAD == "END_FUNCTION") HEAD = "ENDFUNCTION";

    Instr in; in.line = lineNo;

    if      (HEAD == "STRING")   { in.op = OP_STRING; in.a = rest; }
    else if (HEAD == "STRINGLN") { in.op = OP_STRINGLN; in.a = rest; }
    else if (HEAD == "PRINT")    { in.op = OP_PRINT; in.a = rest; }
    else if (HEAD == "DELAY")    { in.op = OP_DELAY; in.n = rest.toInt(); }
    else if (HEAD == "WAIT")     { in.op = OP_WAIT; in.n = rest.toInt(); }
    else if (HEAD == "REPEAT")   { in.op = OP_REPEAT; in.n = rest.toInt(); }
    else if (HEAD == "MOUSE") {
      String t[3]; int nt = splitTokens(rest, t, 3); String sub = nt ? t[0] : ""; sub.toUpperCase();
      if      (sub == "MOVE")  { in.op = OP_MOUSE_MOVE; in.n = (nt>1?t[1].toInt():0); in.m = (nt>2?t[2].toInt():0); }
      else if (sub == "CLICK") { in.op = OP_MOUSE_CLICK; in.a = (nt>1?t[1]:"LEFT"); }
    }
    else if (HEAD == "VAR") {
      int eq = rest.indexOf('='); if (eq >= 0) {
        String name = clean(rest.substring(0, eq)); if (name.startsWith("$")) name = name.substring(1);
        in.op = OP_VAR; in.a = name; in.b = clean(rest.substring(eq + 1));
      }
    }
    else if (HEAD == "MATH") {
      int eq = rest.indexOf('='); if (eq >= 0) {
        String name = clean(rest.substring(0, eq)); if (name.startsWith("$")) name = name.substring(1);
        in.op = OP_MATH; in.a = name; in.b = clean(rest.substring(eq + 1));
      }
    }
    else if (HEAD == "LOOP") {
      in.op = OP_LOOP; in.n = rest.toInt();
      stk.push_back({OP_LOOP, (int)program.size()});
    }
    else if (HEAD == "ENDLOOP") {
      in.op = OP_ENDLOOP;
      if (!stk.empty()) { program[stk.back().idx].j1 = program.size(); in.j1 = stk.back().idx; program[program.size()].j1 = stk.back().idx; stk.pop_back(); }
    }
    else {
      uint8_t mk;
      if (modKey(head, mk)) { in.op = OP_COMBO; in.a = line; }
      else                  { in.op = OP_KEY;   in.a = head; }
    }

    int idx = program.size();
    program.push_back(in);
  }
}

static void emitCombo(const String &line) {
  String t[6]; int nt = splitTokens(line, t, 6);
  uint8_t code;
  for (int k = 0; k < nt; k++) {
    if (modKey(t[k], code))          Keyboard.press(code);
    else if (resolveKey(t[k], code)) Keyboard.press(code);
  }
  delay(10);
  Keyboard.releaseAll();
}

static void runOne(const Instr &in) {
  if (stopRequested) return;
  switch (in.op) {
    case OP_STRING: {
      String s = expand(in.a);
      if (stringDelay <= 0) Keyboard.print(s);
      else for (int k = 0; k < (int)s.length(); k++) { Keyboard.print(s[k]); delay(stringDelay); }
      break;
    }
    case OP_STRINGLN: {
      String s = expand(in.a);
      if (stringDelay <= 0) Keyboard.print(s);
      else for (int k = 0; k < (int)s.length(); k++) { Keyboard.print(s[k]); delay(stringDelay); }
      Keyboard.write(KEY_RETURN);
      break;
    }
    case OP_KEY:       { uint8_t c; if (resolveKey(in.a, c)) Keyboard.write(c); break; }
    case OP_COMBO:     emitCombo(in.a); break;
    case OP_DELAY:     delay(constrain(in.n, 0, MAX_DELAY_MS)); break;
    case OP_VAR:       vars[in.a] = expand(in.b); break;
    case OP_MATH:      vars[in.a] = String(evalExpr(in.b)); break;
    case OP_MOUSE_MOVE:  Mouse.move((signed char)in.n, (signed char)in.m); break;
    case OP_MOUSE_CLICK: { String b = in.a; b.toUpperCase();
                          if      (b == "RIGHT")  Mouse.click(MOUSE_RIGHT);
                          else if (b == "MIDDLE") Mouse.click(MOUSE_MIDDLE);
                          else                    Mouse.click(MOUSE_LEFT); break; }
    default: break;
  }
}

struct LoopRT { int back; long remaining; };

static void execute() {
  stopRequested = false;
  std::vector<LoopRT> loops;
  int pc = 0;
  long steps = 0;

  while (pc >= 0 && pc < (int)program.size()) {
    if (stopRequested) break;
    if (++steps > MAX_STEPS) break;
    const Instr &in = program[pc];

    switch (in.op) {
      case OP_LOOP:
        if (in.n <= 0) pc = in.j1 + 1;
        else { loops.push_back({pc + 1, in.n}); pc++; }
        break;
      case OP_ENDLOOP:
        if (!loops.empty()) {
          if (--loops.back().remaining > 0) pc = loops.back().back;
          else { loops.pop_back(); pc++; }
        } else pc++;
        break;
      default:
        runOne(in);
        delay(defaultDelay);
        pc++;
        break;
    }
  }
  Keyboard.releaseAll();
}

void startUSB(const String &dev) {
  USB.begin(); Keyboard.begin(); Mouse.begin(); delay(900);
}

void DuckyScript(const String &script) {
  if (script == "ABORT") {
    stopRequested = true;
    Keyboard.releaseAll();
    return;
  }
  parse(script);
  execute();
}

#endif