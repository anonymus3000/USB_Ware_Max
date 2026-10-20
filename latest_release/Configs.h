#ifndef CONFIGS_H
#define CONFIGS_H

#include <Arduino.h>

const byte DNS_PORT = 53;

String  inputSSID   = "USB Ware";                                 
String  inputPASS   = "HID1000#";                                 
int     inputCH     = 6;                                          
bool    inputHidden = false;                                      
int     maxClients  = 1;                                          
uint8_t MAC[6]      = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02};       
IPAddress local_IP(10, 0, 0, 1);                                  
IPAddress gateway(10, 0, 0, 1);                                   
IPAddress subnet(255, 255, 255, 0);                               

String  accessKey   = "";

const char MainPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>USB://WARE - Control Hub</title>
<style>
  :root{
    --bg:#04070a; --panel:#070d0c; --panel2:#0a1311; --edge:#103324;
    --fg:#4dffb0; --fg-dim:#1f8a5c; --ghost:#0e3a28;
    --amber:#ffb454; --err:#ff5d6c; --info:#5cd0ff;
    --glow:0 0 6px rgba(77,255,176,.35);
    --mono:ui-monospace,"SFMono-Regular",Menlo,Consolas,monospace;
  }
  *{box-sizing:border-box}
  body{
    margin:0; background:var(--bg); color:var(--fg);
    font-family:var(--mono); font-size:13px; line-height:1.4;
    padding:10px; min-height:100vh;
  }
  .wrap{max-width:1100px;margin:0 auto}
  header{display:flex;align-items:center;gap:14px;flex-wrap:wrap;
    border:1px solid var(--edge);background:var(--panel);
    padding:10px 14px;border-radius:8px;margin-bottom:10px}
  .brand{font-weight:700;letter-spacing:2px}
  .brand b{color:var(--amber)}
  
  .grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
  @media(max-width:900px){.grid{grid-template-columns:1fr}}
  
  .card{border:1px solid var(--edge);background:var(--panel);border-radius:8px;overflow:hidden;margin-bottom:10px}
  .bar{display:flex;align-items:center;gap:6px;padding:7px 10px;border-bottom:1px solid var(--edge);background:var(--panel2);flex-wrap:wrap}
  .tag{color:var(--fg-dim);font-size:12px;letter-spacing:1px;margin-right:auto}
  
  button,select,.fbtn{font-family:var(--mono);font-size:12px;cursor:pointer;
    background:transparent;color:var(--fg);border:1px solid var(--ghost);
    border-radius:5px;padding:5px 9px;transition:.15s}
  button:hover{border-color:var(--fg);box-shadow:var(--glow);background:rgba(77,255,176,.06)}
  button.run{border-color:var(--fg);color:#04110b;background:var(--fg);font-weight:700}
  button.abort{border-color:var(--err);color:var(--err);background:rgba(255,93,108,0.1);font-weight:700}
  button.abort:hover{background:var(--err);color:#fff;box-shadow:0 0 8px rgba(255,93,108,0.5)}
  
  /* Mouse Input Area */
  .mouse-area{display:flex;flex-direction:column;align-items:center;padding:10px;gap:8px}
  #joystickCanvas{touch-action:none;background:var(--bg);border:1px solid var(--edge);border-radius:50%;cursor:pointer}
  #trackpadArea{width:100%;height:120px;background:var(--bg);border:1px dashed var(--edge);border-radius:6px;
    display:none;align-items:center;justify-content:center;color:var(--fg-dim);cursor:crosshair;user-select:none;touch-action:none}
  
  .mouse-controls{display:flex;gap:6px;width:100%;margin-top:4px;align-items:center}
  .mouse-btns{display:flex;gap:4px;flex:1}
  .mouse-btns button{flex:1;padding:7px}
  .sens-box{display:flex;align-items:center;gap:4px;font-size:11px;color:var(--fg-dim)}
  .sens-box input{width:65px}

  /* Shortcuts Matrix */
  .combo-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(90px,1fr));gap:5px;padding:8px}
  .combo-grid button{padding:7px 4px;font-size:11px;background:var(--panel2);border-color:var(--edge);color:var(--amber)}

  /* Mobile Live Sync Type Box */
  .type-box-wrap{padding:8px}
  #liveType{width:100%;background:var(--bg);border:1px solid var(--ghost);color:var(--fg);padding:8px;border-radius:5px;font-family:var(--mono);font-size:16px}

  /* Editor & Console */
  #editor{width:100%;height:140px;background:var(--bg);color:var(--fg);border:0;padding:8px;font-family:var(--mono);resize:vertical}
  #log{height:110px;overflow:auto;padding:8px;background:var(--bg);font-size:12px}
  .l{margin-bottom:2px}
  .t{color:var(--ghost)} .ok{color:var(--fg)} .er{color:var(--err)} .nf{color:var(--info)} .wn{color:var(--amber)}
  
  .slotrow{display:flex;gap:6px;margin-bottom:6px}
  .slotrow input, .slotrow select{flex:1;background:var(--bg);border:1px solid var(--ghost);color:var(--fg);padding:5px 8px;border-radius:4px}

  details.ref{margin-bottom:10px;border:1px solid var(--edge);border-radius:8px;background:var(--panel)}
  details.ref>summary{cursor:pointer;padding:8px 12px;color:var(--fg-dim);letter-spacing:1px;list-style:none}
  details.ref>summary::before{content:"[+] ";color:var(--fg)}
  details.ref[open]>summary::before{content:"[-] "}
  .setgrid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:8px;padding:0 12px 12px}
  .setgrid label{display:flex;flex-direction:column;gap:3px;font-size:11px;color:var(--fg-dim)}
  .setgrid input{background:var(--bg);border:1px solid var(--ghost);border-radius:5px;color:var(--fg);padding:6px;font-family:var(--mono)}
  
  input[type=file]{display:none}
</style>
</head>
<body>
<div class="wrap">
  <header>
    <span class="brand">USB://<b>WARE</b></span>
    <button class="abort" onclick="sendCommand('ABORT'); log('[WARN] Abort signal triggered', 'wn');" style="margin-left:auto">⛔ ABORT SCRIPT</button>
  </header>

  <div class="grid">
    <!-- Left Column -->
    <div>
      <section class="card">
        <div class="bar">
          <span class="tag">MACRO EDITOR</span>
          <label class="fbtn" for="fileInput">UPLOAD</label>
          <input type="file" id="fileInput" accept=".txt,.duck,.dd" onchange="uploadFile(event)">
          <button onclick="exportFile()">SAVE PC</button>
          <button class="run" onclick="runScript()">RUN</button>
        </div>
        <textarea id="editor" placeholder="STRING Hello World&#10;ENTER"></textarea>
      </section>

      <section class="card">
        <div class="bar"><span class="tag">SAVED SLOTS</span></div>
        <div style="padding:8px;">
          <div class="slotrow">
            <select id="scriptSelector" onchange="loadSelectedScript()">
              <option value="">-- Select Saved Script --</option>
            </select>
          </div>
          <div class="slotrow">
            <input id="slotName" placeholder="Slot Name">
            <button onclick="saveSlot()">SAVE</button>
            <button onclick="deleteSlot()" style="color:var(--err)">DEL</button>
          </div>
        </div>
      </section>

      <section class="card">
        <div class="bar"><span class="tag">QUICK SHORTCUTS</span></div>
        <div class="combo-grid">
          <button onclick="sendCommand('CTRL c')">CTRL+C</button>
          <button onclick="sendCommand('CTRL v')">CTRL+V</button>
          <button onclick="sendCommand('CTRL x')">CTRL+X</button>
          <button onclick="sendCommand('CTRL a')">CTRL+A</button>
          <button onclick="sendCommand('CTRL s')">CTRL+S</button>
          <button onclick="sendCommand('ALT f4')">ALT+F4</button>
          <button onclick="sendCommand('ALT Tab')">ALT+TAB</button>
          <button onclick="sendCommand('GUI d')">WIN+D</button>
          <button onclick="sendCommand('CTRL SHIFT ESC')">TASKMGR</button>
          <button onclick="sendCommand('GUI r')">RUN</button>
        </div>
      </section>

      <section class="card">
        <div class="bar"><span class="tag">OFFICE APPS LAUNCHER</span></div>
        <div class="combo-grid">
          <button onclick="sendCommand('GUI CTRL ALT SHIFT w')">WORD</button>
          <button onclick="sendCommand('GUI CTRL ALT SHIFT x')">EXCEL</button>
          <button onclick="sendCommand('GUI CTRL ALT SHIFT p')">POWERPOINT</button>
          <button onclick="sendCommand('GUI CTRL ALT SHIFT o')">OUTLOOK</button>
          <button onclick="sendCommand('GUI CTRL ALT SHIFT t')">TEAMS</button>
          <button onclick="sendCommand('GUI CTRL ALT SHIFT n')">ONENOTE</button>
        </div>
      </section>

      <!-- System & Media Controls placed underneath Office App Launcher -->
      <section class="card">
        <div class="bar"><span class="tag">SYSTEM & MEDIA CONTROLS</span></div>
        <div class="combo-grid">
          <button onclick="sendCommand('GUI r\nDELAY 500\nSTRING rundll32.exe powrprof.dll,SetSuspendState 0,1,0\nENTER')">💤 SLEEP</button>
          <button onclick="sendCommand('GUI r\nDELAY 500\nSTRING shutdown /r /t 0\nENTER')">🔄 RESTART</button>
          <button onclick="sendCommand('GUI e')">📁 EXPLORER</button>
          <button onclick="sendCommand('GUI r\nDELAY 500\nSTRING calc\nENTER')">🧮 CALC</button>
          <button onclick="sendCommand('GUI r\nDELAY 500\nSTRING powershell -c \"(New-Object -ComObject WScript.Shell).SendKeys([char]173)\"\nENTER')">🔇 MUTE</button>
          <button onclick="sendCommand('GUI r\nDELAY 500\nSTRING powershell -c \"(New-Object -ComObject WScript.Shell).SendKeys([char]174)\"\nENTER')">🔉 VOL -</button>
          <button onclick="sendCommand('GUI r\nDELAY 500\nSTRING powershell -c \"(New-Object -ComObject WScript.Shell).SendKeys([char]175)\"\nENTER')">🔊 VOL +</button>
        </div>
      </section>
    </div>

    <!-- Right Column -->
    <div>
      <!-- Mobile Direct-Type at the top of the right column -->
      <section class="card">
        <div class="bar"><span class="tag">MOBILE DIRECT-TYPE</span></div>
        <div class="type-box-wrap">
          <input id="liveType" type="text" placeholder="Tap to type live..." autocomplete="off" autocapitalize="off">
        </div>
      </section>

      <!-- Virtual Mouse underneath Direct-Type -->
      <section class="card">
        <div class="bar">
          <span class="tag">VIRTUAL MOUSE</span>
          <select id="mouseMode" onchange="toggleMouseMode()" style="padding:2px 5px">
            <option value="joystick">Joystick Mode</option>
            <option value="trackpad">Trackpad Mode</option>
          </select>
        </div>
        <div class="mouse-area">
          <canvas id="joystickCanvas" width="110" height="110"></canvas>
          <div id="trackpadArea">Drag here for trackpad</div>
          <div class="mouse-controls">
            <div class="sens-box">
              Sens: <input type="range" id="sensRange" min="0.5" max="3.0" step="0.5" value="1.0">
            </div>
            <div class="mouse-btns">
              <button onclick="mouseClick('LEFT')">LMB</button>
              <button onclick="mouseClick('MIDDLE')">MMB</button>
              <button onclick="mouseClick('RIGHT')">RMB</button>
            </div>
          </div>
        </div>
      </section>

      <!-- Virtual Keyboard underneath Virtual Mouse -->
      <section class="card">
        <div class="bar"><span class="tag">VIRTUAL KEYBOARD</span></div>
        <div style="padding:6px; display:flex; gap:4px; flex-wrap:wrap; justify-content:center">
          <button class="fbtn" onclick="sendCommand('ESC')">ESC</button>
          <button class="fbtn" onclick="sendCommand('TAB')">TAB</button>
          <button class="fbtn" onclick="sendCommand('BACKSPACE')">BKSP</button>
          <button class="fbtn" onclick="sendCommand('ENTER')">ENTER</button>
          <button class="fbtn" onclick="sendCommand('SPACE')">SPACE</button>
          <button class="fbtn" onclick="sendCommand('UP')">&uarr;</button>
          <button class="fbtn" onclick="sendCommand('DOWN')">&darr;</button>
          <button class="fbtn" onclick="sendCommand('LEFT')">&larr;</button>
          <button class="fbtn" onclick="sendCommand('RIGHT')">&rarr;</button>
        </div>
      </section>
    </div>
  </div>

  <details class="ref" ontoggle="if(this.open)loadSettings()">
    <summary>DEVICE & WIFI SETTINGS</summary>
    <div class="setgrid">
      <label>SSID<input id="s_ssid"></label>
      <label>PASSWORD<input id="s_pass"></label>
      <label>CHANNEL<input id="s_ch" type="number" min="1" max="13"></label>
      <label>MAX CLIENTS<input id="s_max" type="number" min="1" max="8"></label>
      <label>IP ADDRESS<input id="s_ip"></label>
      <label>GATEWAY<input id="s_gw"></label>
      <label>STRING DELAY (ms)<input id="s_sd" type="number" min="0"></label>
    </div>
    <div style="padding:0 12px 12px; display:flex; gap:8px;">
      <button class="run" onclick="saveSettings()">SAVE SETTINGS</button>
      <button onclick="loadSettings()">RELOAD</button>
      <span id="setStat" style="font-size:11px;color:var(--fg-dim);align-self:center;margin-left:auto"></span>
    </div>
  </details>

  <details class="ref">
    <summary>EXPANDED SYNTAX CHEAT SHEET & AI PROMPT</summary>
    <div style="padding:12px;font-size:11px;color:var(--fg-dim);line-height:1.6">
      <b style="color:var(--amber)">Text Input:</b> <code>STRING text</code> | <code>STRINGLN text</code> | <code>DELAY 500</code><br>
      <b style="color:var(--amber)">Hotkeys & Apps:</b> <code>CTRL c</code> | <code>ALT f4</code> | <code>GUI r</code> | <code>GUI CTRL ALT SHIFT w</code> (Word)<br>
      <b style="color:var(--amber)">Mouse:</b> <code>MOUSE MOVE x y</code> | <code>MOUSE CLICK LEFT/RIGHT/MIDDLE</code><br>
      <b style="color:var(--amber)">Flow Control:</b> <code>VAR $x = 5</code> | <code>LOOP 3 ... ENDLOOP</code> | <code>WHILE x > 0 ... ENDWHILE</code>
      <hr style="border:0;border-top:1px solid var(--edge);margin:8px 0">
      <b style="color:var(--fg)">AI Prompt Template to Generate Scripts:</b>
      <div style="background:var(--bg);padding:8px;border-radius:4px;margin-top:4px;user-select:all;color:var(--fg)">
        Act as an ESP32 USB HID DuckyScript developer. Write a script for: [DESCRIBE YOUR TASK HERE]. Use only valid commands: STRING, STRINGLN, DELAY ms, ENTER, TAB, ESC, CTRL/ALT/GUI/SHIFT combos, MOUSE MOVE x y, MOUSE CLICK LEFT/RIGHT, LOOP n ... ENDLOOP.
      </div>
    </div>
  </details>

  <section class="card">
    <div class="bar"><span class="tag">CONSOLE</span><button onclick="document.getElementById('log').innerHTML=''">CLR</button></div>
    <div id="log"></div>
  </section>
</div>

<script>
function now(){var d=new Date();return d.toTimeString().slice(0,8);}
function log(msg, cls){
  var box=document.getElementById('log');
  var line=document.createElement('div'); line.className='l';
  line.innerHTML='<span class="t">'+now()+' </span><span class="'+(cls||'ok')+'">'+msg+'</span>';
  box.appendChild(line); box.scrollTop=box.scrollHeight;
}

async function sendCommand(scriptBody) {
  log('[INFO] Transmitting action...', 'nf');
  try {
    let res = await fetch('/set', {
      method: 'POST',
      headers: {'Content-Type': 'application/x-www-form-urlencoded'},
      body: 'customScript=' + encodeURIComponent(scriptBody)
    });
    let txt = await res.text();
    if(res.ok) log('[OK] ' + (txt || 'Executed successfully'), 'ok');
    else log('[ERR] HTTP ' + res.status + ': ' + txt, 'er');
  } catch(e) { log('[ERR] Link failure: ' + e, 'er'); }
}

function runScript() {
  let val = document.getElementById('editor').value;
  if(!val.trim()){ log('[WARN] Editor is empty', 'wn'); return; }
  sendCommand(val);
}

function mouseClick(btn) {
  sendCommand(`MOUSE CLICK ${btn}`);
}

function toggleMouseMode() {
  let mode = document.getElementById('mouseMode').value;
  let joy = document.getElementById('joystickCanvas');
  let track = document.getElementById('trackpadArea');
  if(mode === 'trackpad') {
    joy.style.display = 'none';
    track.style.display = 'flex';
  } else {
    joy.style.display = 'block';
    track.style.display = 'none';
  }
}

let trackArea = document.getElementById('trackpadArea');
let isTracking = false, tLastX = 0, tLastY = 0;
trackArea.addEventListener('pointerdown', e => { isTracking = true; tLastX = e.clientX; tLastY = e.clientY; });
window.addEventListener('pointerup', () => { isTracking = false; });
window.addEventListener('pointermove', e => {
  if(!isTracking || document.getElementById('mouseMode').value !== 'trackpad') return;
  let sens = parseFloat(document.getElementById('sensRange').value);
  let dx = Math.round((e.clientX - tLastX) * 1.5 * sens);
  let dy = Math.round((e.clientY - tLastY) * 1.5 * sens);
  tLastX = e.clientX; tLastY = e.clientY;
  if(dx !== 0 || dy !== 0) sendCommand(`MOUSE MOVE ${dx} ${dy}`);
});

const canvas = document.getElementById('joystickCanvas');
const ctx = canvas.getContext('2d');
let cx = 55, cy = 55, r = 40;
let stickX = cx, stickY = cy;
let joystickActive = false;

function drawJoystick(x, y) {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.beginPath(); ctx.arc(cx, cy, r, 0, Math.PI * 2);
  ctx.fillStyle = 'rgba(77,255,176,0.04)'; ctx.fill();
  ctx.strokeStyle = 'var(--edge)'; ctx.lineWidth = 2; ctx.stroke();
  
  ctx.beginPath(); ctx.arc(x, y, 18, 0, Math.PI * 2);
  ctx.fillStyle = joystickActive ? 'var(--fg)' : 'var(--fg-dim)'; ctx.fill();
}
drawJoystick(cx, cy);

function handleJoystickMove(clientX, clientY) {
  let rect = canvas.getBoundingClientRect();
  let dx = clientX - (rect.left + cx);
  let dy = clientY - (rect.top + cy);
  let dist = Math.sqrt(dx*dx + dy*dy);
  if(dist > r) {
    dx = (dx / dist) * r;
    dy = (dy / dist) * r;
  }
  stickX = cx + dx;
  stickY = cy + dy;
  drawJoystick(stickX, stickY);
}

canvas.addEventListener('pointerdown', e => { joystickActive = true; handleJoystickMove(e.clientX, e.clientY); });
window.addEventListener('pointermove', e => { if(joystickActive && document.getElementById('mouseMode').value === 'joystick') handleJoystickMove(e.clientX, e.clientY); });
window.addEventListener('pointerup', () => {
  if(joystickActive) {
    joystickActive = false;
    stickX = cx; stickY = cy;
    drawJoystick(cx, cy);
  }
});

setInterval(() => {
  if(joystickActive && document.getElementById('mouseMode').value === 'joystick') {
    let sens = parseFloat(document.getElementById('sensRange').value);
    let dx = Math.round((stickX - cx) * 0.2 * sens);
    let dy = Math.round((stickY - cy) * 0.2 * sens);
    if(dx !== 0 || dy !== 0) sendCommand(`MOUSE MOVE ${dx} ${dy}`);
  }
}, 50);

/* --- Mobile Direct Type (Fixed re-focus / closing bug) --- */
let lastLen = 0;
let liveInput = document.getElementById('liveType');
liveInput.addEventListener('focus', () => { lastLen = liveInput.value.length; });
liveInput.addEventListener('input', e => {
  let val = e.target.value;
  if(val.length > lastLen) {
    let added = val.slice(lastLen);
    sendCommand(`STRING ${added}`);
  } else if(val.length < lastLen) {
    let diff = lastLen - val.length;
    let bk = "";
    for(let i=0; i<diff; i++) bk += "BACKSPACE\n";
    sendCommand(bk.trim());
  }
  lastLen = val.length;
  // Removed automatic value wiping so mobile keyboard stays open and stable!
});

function uploadFile(e) {
  let f = e.target.files[0]; if(!f) return;
  let r = new FileReader();
  r.onload = function(ev) {
    document.getElementById('editor').value = ev.target.result;
    document.getElementById('slotName').value = f.name.replace(/\.[^/.]+$/, "");
    log('[INFO] Uploaded file: ' + f.name, 'nf');
  };
  r.readAsText(f);
  e.target.value = '';
}

function exportFile() {
  let val = document.getElementById('editor').value;
  let blob = new Blob([val], {type:'text/plain'});
  let a = document.createElement('a');
  a.href = URL.createObjectURL(blob); a.download = 'payload.duck'; a.click();
  URL.revokeObjectURL(a.href);
  log('[INFO] Exported payload to PC', 'nf');
}

function getSlots() {
  try { return JSON.parse(localStorage.getItem('usbware_scripts') || '{}'); } catch(e) { return {}; }
}
function renderSlots() {
  let s = getSlots(), sel = document.getElementById('scriptSelector');
  sel.innerHTML = '<option value="">-- Select Saved Script --</option>';
  for(let k in s) {
    let opt = document.createElement('option'); opt.value = k; opt.textContent = k;
    sel.appendChild(opt);
  }
}
function saveSlot() {
  let name = document.getElementById('slotName').value.trim();
  let code = document.getElementById('editor').value.trim();
  if(!name || !code) { log('[WARN] Enter slot name and script code', 'wn'); return; }
  let s = getSlots(); s[name] = code;
  localStorage.setItem('usbware_scripts', JSON.stringify(s));
  renderSlots();
  log('[OK] Saved slot: ' + name, 'ok');
}
function loadSelectedScript() {
  let sel = document.getElementById('scriptSelector').value;
  if(!sel) return;
  let s = getSlots();
  if(s[sel]) {
    document.getElementById('editor').value = s[sel];
    document.getElementById('slotName').value = sel;
    log('[INFO] Loaded slot: ' + sel, 'nf');
  }
}
function deleteSlot() {
  let sel = document.getElementById('scriptSelector').value;
  if(!sel) return;
  let s = getSlots(); delete s[sel];
  localStorage.setItem('usbware_scripts', JSON.stringify(s));
  renderSlots();
  document.getElementById('editor').value = '';
  document.getElementById('slotName').value = '';
  log('[WARN] Deleted slot: ' + sel, 'wn');
}

async function loadSettings() {
  try {
    let r = await fetch('/api/settings');
    if(r.ok) {
      let s = await r.json();
      document.getElementById('s_ssid').value = s.ssid;
      document.getElementById('s_pass').value = s.pass;
      document.getElementById('s_ch').value = s.ch;
      document.getElementById('s_max').value = s.max;
      document.getElementById('s_ip').value = s.ip;
      document.getElementById('s_gw').value = s.gw;
      document.getElementById('s_sd').value = s.sdelay;
      document.getElementById('setStat').textContent = `Heap: ${s.heap}B`;
      log('[INFO] Settings loaded', 'nf');
    }
  } catch(e) { log('[ERR] Settings error: ' + e, 'er'); }
}

async function saveSettings() {
  let p = new URLSearchParams();
  p.set('ssid', document.getElementById('s_ssid').value);
  p.set('pass', document.getElementById('s_pass').value);
  p.set('ch', document.getElementById('s_ch').value);
  p.set('max', document.getElementById('s_max').value);
  p.set('ip', document.getElementById('s_ip').value);
  p.set('gw', document.getElementById('s_gw').value);
  p.set('sdelay', document.getElementById('s_sd').value);
  p.set('reboot', '1');
  try {
    let r = await fetch('/api/settings', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:p.toString()});
    if(r.ok) log('[OK] Settings saved. Rebooting...', 'ok');
  } catch(e) { log('[ERR] Save error: ' + e, 'er'); }
}

renderSlots();
</script>
</body>
</html>
)rawliteral";

#endif
