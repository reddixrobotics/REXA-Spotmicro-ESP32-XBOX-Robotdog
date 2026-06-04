#include "web_dashboard.h"
#include <WiFi.h>

WebServer server(80);

// Implement the external hook declared in gait.h/gait.cpp
void handleNetworkAndBluetooth() {
  server.handleClient();
  updateXbox();
}

// Stunning glassmorphic HTML dashboard
const char HTML_PAGE[] PROGMEM = R"rawHTML(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <title>REXA Quadruped Dashboard</title>
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;800&family=Share+Tech+Mono&display=swap');
    :root {
      --bg: #0b0c16;
      --panel: rgba(18, 20, 38, 0.65);
      --border: rgba(255, 255, 255, 0.08);
      --border-glowing: rgba(0, 229, 255, 0.3);
      --accent: #00e5ff;
      --accent-purple: #7c4dff;
      --accent-green: #00ff88;
      --accent-red: #ff2056;
      --text: #e2e8f0;
      --dim: #718096;
      --glow: 0 0 15px rgba(0, 229, 255, 0.4);
    }
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      -webkit-tap-highlight-color: transparent;
      user-select: none;
    }
    body {
      background: var(--bg);
      background-image: radial-gradient(circle at 50% 50%, #151833 0%, #0b0c16 80%);
      color: var(--text);
      font-family: 'Outfit', sans-serif;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      overflow-x: hidden;
      padding: 10px 10px 30px;
    }
    header {
      text-align: center;
      margin: 15px 0 10px;
      z-index: 2;
    }
    .brand-logo {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      margin-bottom: 6px;
    }
    .brand-logo svg {
      width: 44px;
      height: 44px;
      filter: drop-shadow(0 2px 8px rgba(107, 26, 26, 0.4));
    }
    .brand-name {
      font-family: 'Outfit', sans-serif;
      font-weight: 800;
      font-size: 1.2rem;
      color: #a33;
      letter-spacing: 2px;
      line-height: 1.1;
      text-shadow: 0 1px 6px rgba(107, 26, 26, 0.3);
    }
    .brand-name small {
      display: block;
      font-size: 0.55rem;
      font-weight: 400;
      letter-spacing: 3px;
      color: var(--dim);
      margin-top: 2px;
    }
    header h1 {
      font-size: 2.2rem;
      font-weight: 800;
      letter-spacing: 4px;
      background: linear-gradient(135deg, #00e5ff, #7c4dff);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      filter: drop-shadow(0 2px 8px rgba(0, 229, 255, 0.2));
      display: inline-block;
    }
    header p {
      font-family: 'Share Tech Mono', monospace;
      font-size: 0.8rem;
      color: var(--accent);
      letter-spacing: 2px;
      margin-top: 4px;
      text-transform: uppercase;
    }
    
    /* Navigation Tabs */
    .tabs {
      display: flex;
      background: var(--panel);
      backdrop-filter: blur(12px);
      border: 1px solid var(--border);
      border-radius: 30px;
      padding: 4px;
      margin-bottom: 20px;
      width: 100%;
      max-width: 420px;
      z-index: 2;
    }
    .tab-btn {
      flex: 1;
      background: transparent;
      border: none;
      color: var(--dim);
      font-family: 'Outfit', sans-serif;
      font-weight: 600;
      font-size: 0.9rem;
      padding: 10px 5px;
      border-radius: 25px;
      cursor: pointer;
      transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    }
    .tab-btn.active {
      background: linear-gradient(135deg, var(--accent), var(--accent-purple));
      color: #fff;
      box-shadow: var(--glow);
    }
    
    /* Pages Container */
    .page-container {
      width: 100%;
      max-width: 420px;
      z-index: 2;
    }
    .page {
      display: none;
      animation: fadeIn 0.4s ease-out forwards;
    }
    .page.active {
      display: flex;
      flex-direction: column;
      gap: 15px;
    }
    @keyframes fadeIn {
      from { opacity: 0; transform: translateY(10px); }
      to { opacity: 1; transform: translateY(0); }
    }
    
    /* Glassmorphic Panel Card */
    .card {
      background: var(--panel);
      backdrop-filter: blur(15px);
      border: 1px solid var(--border);
      border-radius: 20px;
      padding: 20px;
      box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.3);
      position: relative;
      overflow: hidden;
      transition: border-color 0.3s;
    }
    .card::before {
      content: '';
      position: absolute;
      top: 0; left: -100%;
      width: 50%; height: 100%;
      background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.05), transparent);
      transition: 0.5s;
      pointer-events: none;
    }
    .card:hover::before {
      left: 150%;
    }
    .card-title {
      font-family: 'Share Tech Mono', monospace;
      font-size: 0.95rem;
      color: var(--accent);
      letter-spacing: 2px;
      margin-bottom: 15px;
      border-bottom: 1px solid var(--border);
      padding-bottom: 8px;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    
    /* Status Badge */
    .badge {
      font-family: 'Share Tech Mono', monospace;
      font-size: 0.75rem;
      font-weight: bold;
      padding: 3px 8px;
      border-radius: 6px;
      border: 1px solid;
    }
    .badge.green { color: var(--accent-green); border-color: rgba(0, 255, 136, 0.3); background: rgba(0, 255, 136, 0.05); }
    .badge.purple { color: var(--accent-purple); border-color: rgba(124, 77, 255, 0.3); background: rgba(124, 77, 255, 0.05); }
    .badge.dim { color: var(--dim); border-color: var(--border); }
    
    /* Operation Controls */
    .status-display {
      font-family: 'Share Tech Mono', monospace;
      font-size: 1.5rem;
      text-align: center;
      color: #fff;
      text-shadow: 0 0 10px rgba(255,255,255,0.2);
      margin: 10px 0;
      letter-spacing: 1px;
    }
    
    .ctrl-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
      margin-top: 15px;
    }
    .btn {
      background: rgba(255, 255, 255, 0.03);
      border: 1px solid var(--border);
      color: var(--text);
      font-family: 'Outfit', sans-serif;
      font-weight: 600;
      font-size: 0.95rem;
      padding: 14px 10px;
      border-radius: 12px;
      cursor: pointer;
      transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
    }
    .btn:active {
      transform: scale(0.95);
    }
    .btn.accent-btn {
      background: linear-gradient(135deg, rgba(0, 229, 255, 0.1), rgba(124, 77, 255, 0.1));
      border-color: var(--accent-purple);
      color: #fff;
    }
    .btn.accent-btn:hover {
      box-shadow: 0 0 12px rgba(124, 77, 255, 0.3);
      border-color: var(--accent);
    }
    .btn.danger-btn {
      border-color: rgba(255, 32, 86, 0.4);
      color: var(--accent-red);
    }
    
    /* Touch Joystick Canvas Container */
    .joystick-container {
      display: flex;
      justify-content: center;
      align-items: center;
      margin: 20px 0 10px;
      position: relative;
    }
    #joyBase {
      width: 200px;
      height: 200px;
      border-radius: 50%;
      background: radial-gradient(circle, rgba(18, 20, 38, 0.9) 0%, rgba(11, 12, 22, 0.95) 75%);
      border: 2px solid var(--border);
      box-shadow: inset 0 0 20px rgba(0, 0, 0, 0.8), 0 0 15px rgba(0, 229, 255, 0.05);
      position: relative;
      cursor: pointer;
      touch-action: none;
    }
    #joyBase::before {
      content: '';
      position: absolute;
      inset: 20px;
      border-radius: 50%;
      border: 1px dashed rgba(0, 229, 255, 0.15);
      pointer-events: none;
    }
    #joyBase::after {
      content: '';
      position: absolute;
      inset: 50px;
      border-radius: 50%;
      border: 1px dashed rgba(124, 77, 255, 0.1);
      pointer-events: none;
    }
    #joyKnob {
      width: 60px;
      height: 60px;
      border-radius: 50%;
      background: radial-gradient(circle at 35% 35%, #1d203f 0%, #0d0e1b 80%);
      border: 2.5px solid var(--accent);
      box-shadow: 0 6px 16px rgba(0,0,0,0.5), var(--glow), inset 0 2px 4px rgba(255,255,255,0.1);
      position: absolute;
      top: 50%; left: 50%;
      transform: translate(-50%, -50%);
      pointer-events: none;
      z-index: 2;
    }
    #joyKnob::after {
      content: '';
      position: absolute;
      top: 50%; left: 50%;
      transform: translate(-50%, -50%);
      width: 10px;
      height: 10px;
      border-radius: 50%;
      background: var(--accent);
      box-shadow: var(--glow);
    }
    .joy-axis-labels {
      display: flex;
      justify-content: space-between;
      width: 100%;
      max-width: 240px;
      margin: 0 auto;
      font-family: 'Share Tech Mono', monospace;
      font-size: 0.65rem;
      color: var(--dim);
    }
    
    /* Telemetry Display */
    .telemetry-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 12px;
      font-family: 'Share Tech Mono', monospace;
    }
    .telemetry-label {
      color: var(--dim);
      font-size: 0.85rem;
    }
    .telemetry-value {
      color: #fff;
      font-size: 0.95rem;
      font-weight: 600;
    }
    .telemetry-value.glowing {
      color: var(--accent-green);
      text-shadow: 0 0 8px rgba(0,255,136,0.3);
    }
    
    /* Visual Pitch/Roll Leveler Gauges */
    .gauge-container {
      display: flex;
      gap: 15px;
      margin: 10px 0;
    }
    .level-gauge {
      flex: 1;
      height: 70px;
      background: rgba(0,0,0,0.25);
      border: 1px solid var(--border);
      border-radius: 12px;
      position: relative;
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      overflow: hidden;
    }
    .level-line {
      position: absolute;
      width: 100%;
      height: 2px;
      background: var(--accent);
      left: 0;
      transform-origin: center center;
      transition: transform 0.1s ease-out;
      box-shadow: var(--glow);
    }
    .level-text {
      z-index: 2;
      font-family: 'Share Tech Mono', monospace;
      font-size: 0.75rem;
      color: var(--dim);
    }
    .level-val {
      z-index: 2;
      font-family: 'Share Tech Mono', monospace;
      font-size: 1rem;
      font-weight: bold;
      color: #fff;
    }
    
    /* Calibration Matrix */
    .calib-grid {
      display: flex;
      flex-direction: column;
      gap: 16px;
    }
    .leg-section {
      background: rgba(255,255,255,0.015);
      border: 1px solid rgba(255,255,255,0.03);
      border-radius: 14px;
      padding: 12px;
    }
    .leg-title {
      font-family: 'Share Tech Mono', monospace;
      font-size: 0.85rem;
      color: var(--accent-purple);
      margin-bottom: 8px;
      font-weight: bold;
      letter-spacing: 1px;
    }
    .joint-row {
      display: flex;
      align-items: center;
      gap: 12px;
      margin-bottom: 8px;
    }
    .joint-row:last-child {
      margin-bottom: 0;
    }
    .joint-label {
      font-family: 'Share Tech Mono', monospace;
      width: 50px;
      font-size: 0.8rem;
      color: var(--dim);
    }
    .joint-slider {
      flex: 1;
      height: 6px;
      -webkit-appearance: none;
      background: rgba(255, 255, 255, 0.05);
      border-radius: 3px;
      outline: none;
    }
    .joint-slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 18px;
      height: 18px;
      border-radius: 50%;
      background: var(--accent);
      cursor: pointer;
      box-shadow: var(--glow);
      transition: background 0.1s;
    }
    .joint-slider::-webkit-slider-thumb:active {
      background: var(--accent-purple);
    }
    .joint-val {
      font-family: 'Share Tech Mono', monospace;
      width: 42px;
      text-align: right;
      font-size: 0.85rem;
      font-weight: bold;
      color: var(--accent);
    }
  </style>
</head>
<body>

  <header>
    <div class="brand-logo">
      <svg viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
        <rect x="10" y="5" width="50" height="90" rx="8" fill="#6B1A1A"/>
        <rect x="10" y="72" width="18" height="23" rx="4" fill="#6B1A1A"/>
        <path d="M35 5 h25 q20 0 20 20 v5 q0 18 -20 18 h-5 l20 47 h-18 l-18 -45 v-5 h16 q8 0 8 -8 v-6 q0 -8 -8 -8 h-20 z" fill="#6B1A1A"/>
        <line x1="38" y1="30" x2="52" y2="16" stroke="#e2e8f0" stroke-width="4" stroke-linecap="round"/>
        <line x1="52" y1="16" x2="66" y2="22" stroke="#e2e8f0" stroke-width="4" stroke-linecap="round"/>
        <circle cx="38" cy="30" r="4" fill="#e2e8f0"/>
        <circle cx="52" cy="16" r="3" fill="#e2e8f0"/>
        <path d="M66 18 l6 -4 l-2 8 l-6 2 z" fill="#e2e8f0"/>
      </svg>
      <div class="brand-name">
        REDDIX ROBOTICS
        <small>ESTD 2025, INDIA</small>
      </div>
    </div>
    <h1>REXA ROBOT DOG</h1>
    <p>Quadruped Intelligent Stance Controller</p>
  </header>

  <!-- Nav Tabs -->
  <div class="tabs">
    <button class="tab-btn active" onclick="switchTab('teleop')">CONTROLLER</button>
    <button class="tab-btn" onclick="switchTab('calib')">CALIBRATION</button>
    <button class="tab-btn" onclick="switchTab('telemetry')">TELEMETRY</button>
  </div>

  <!-- Main Pages -->
  <div class="page-container">
    
    <!-- Tab 1: Teleop / Controller -->
    <div id="page-teleop" class="page active">
      <div class="card">
        <div class="card-title">
          <span>🎮 TELEOPERATION STANCE</span>
          <span class="badge purple" id="statusBadge">STAND</span>
        </div>
        
        <div class="status-display" id="poseDisplay">STANDING</div>
        
        <!-- Joystick Canvas -->
        <div class="joystick-container">
          <div id="joyBase">
            <div id="joyKnob"></div>
          </div>
        </div>
        <div class="joy-axis-labels">
          <span>◄ LEFT / RIGHT ►</span>
          <span>▲ FWD / BWD ▼</span>
        </div>
        
        <div class="ctrl-grid">
          <button class="btn accent-btn" onclick="sendCmd('s')">■ STAND</button>
          <button class="btn accent-btn" onclick="sendCmd('x')">⬇ SIT</button>
          <button class="btn" onclick="sendCmd('u')">▲ HEIGHT +</button>
          <button class="btn" onclick="sendCmd('d')">▼ HEIGHT -</button>
        </div>
        
        <div class="ctrl-grid" style="grid-template-columns: 1fr; margin-top: 12px;">
          <button class="btn danger-btn" onclick="sendCmd('g')">🐾 WAVE / GREET POSE</button>
        </div>
      </div>
    </div>
    
    <!-- Tab 2: Calibration Matrix -->
    <div id="page-calib" class="page">
      <div class="card">
        <div class="card-title">
          <span>🛠 JOINT CALIBRATION (DEGREES)</span>
          <span class="badge green">LIVE TUNING</span>
        </div>
        
        <div class="calib-grid">
          <!-- Legs Generator -->
          <div class="leg-section" data-leg="0">
            <div class="leg-title">FRONT RIGHT (RF)</div>
            <div class="joint-row"><span class="joint-label">COXA</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(0,0,this.value)"><span class="joint-val">0°</span></div>
            <div class="joint-row"><span class="joint-label">FEMUR</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(0,1,this.value)"><span class="joint-val">0°</span></div>
            <div class="joint-row"><span class="joint-label">TIBIA</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(0,2,this.value)"><span class="joint-val">0°</span></div>
          </div>
          
          <div class="leg-section" data-leg="1">
            <div class="leg-title">BACK RIGHT (BR)</div>
            <div class="joint-row"><span class="joint-label">COXA</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(1,0,this.value)"><span class="joint-val">0°</span></div>
            <div class="joint-row"><span class="joint-label">FEMUR</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(1,1,this.value)"><span class="joint-val">0°</span></div>
            <div class="joint-row"><span class="joint-label">TIBIA</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(1,2,this.value)"><span class="joint-val">0°</span></div>
          </div>
          
          <div class="leg-section" data-leg="2">
            <div class="leg-title">BACK LEFT (BL)</div>
            <div class="joint-row"><span class="joint-label">COXA</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(2,0,this.value)"><span class="joint-val">0°</span></div>
            <div class="joint-row"><span class="joint-label">FEMUR</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(2,1,this.value)"><span class="joint-val">0°</span></div>
            <div class="joint-row"><span class="joint-label">TIBIA</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(2,2,this.value)"><span class="joint-val">0°</span></div>
          </div>
          
          <div class="leg-section" data-leg="3">
            <div class="leg-title">FRONT LEFT (FL)</div>
            <div class="joint-row"><span class="joint-label">COXA</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(3,0,this.value)"><span class="joint-val">0°</span></div>
            <div class="joint-row"><span class="joint-label">FEMUR</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(3,1,this.value)"><span class="joint-val">0°</span></div>
            <div class="joint-row"><span class="joint-label">TIBIA</span><input class="joint-slider" type="range" min="-30" max="30" value="0" oninput="tuneJoint(3,2,this.value)"><span class="joint-val">0°</span></div>
          </div>
        </div>
        
        <div style="margin-top: 20px;">
          <button class="btn accent-btn" style="width: 100%;" onclick="saveCalibration()">💾 PERSIST TO FLASH MEMORY (LITTLEFS)</button>
        </div>
      </div>
    </div>
    
    <!-- Tab 3: Telemetry Panel -->
    <div id="page-telemetry" class="page">
      <div class="card">
        <div class="card-title">
          <span>🩺 CLOSED-LOOP MONITORING</span>
          <span class="badge green">TELEMETRY</span>
        </div>
        
        <div class="telemetry-row">
          <span class="telemetry-label">POSE / MOTION STATE:</span>
          <span class="telemetry-value" id="tMotionState">STAND</span>
        </div>
        <div class="telemetry-row">
          <span class="telemetry-label">STANDING HEIGHT STEP:</span>
          <span class="telemetry-value" id="tHeight">LEVEL 2</span>
        </div>
        <div class="telemetry-row">
          <span class="telemetry-label">XBOX CONTROLLER:</span>
          <span class="telemetry-value" id="tXbox">DISCONNECTED</span>
        </div>
        
        <div class="card-title" style="margin-top: 20px; font-size: 0.8rem; border-bottom: none; padding-bottom: 0;">
          <span>📐 ACTIVE GYRO IMU FEEDBACK</span>
        </div>
        
        <div class="gauge-container">
          <div class="level-gauge">
            <div class="level-text">PITCH</div>
            <div class="level-val" id="valPitch">0.0°</div>
            <div class="level-line" id="linePitch"></div>
          </div>
          <div class="level-gauge">
            <div class="level-text">ROLL</div>
            <div class="level-val" id="valRoll">0.0°</div>
            <div class="level-line" id="lineRoll"></div>
          </div>
        </div>
      </div>
    </div>
    
  </div>

  <script>
    // Tab switching
    function switchTab(tabId) {
      document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
      document.querySelectorAll('.page').forEach(page => page.classList.remove('active'));
      
      const targetBtn = Array.from(document.querySelectorAll('.tab-btn')).find(b => b.textContent.toLowerCase() === tabId || b.onclick.toString().includes(tabId));
      if (targetBtn) targetBtn.classList.add('active');
      
      const targetPage = document.getElementById(`page-${tabId}`);
      if (targetPage) targetPage.classList.add('active');

      if (tabId === 'calib') loadCalibration();
    }
    
    // Commands sender
    function sendCmd(c) {
      fetch(`/cmd?c=${c}`).then(r => r.text()).then(txt => {
        console.log("Cmd acknowledged: " + c);
      }).catch(err => console.error(err));
    }
    
    // Joystick Touch Operations
    const base = document.getElementById('joyBase');
    const knob = document.getElementById('joyKnob');
    const RADIUS = 70; // boundaries for knob drag
    let originX = 0, originY = 0, active = false, jX = 0, jY = 0, sendTimer = null;
    
    function processJoy(cx, cy) {
      let dx = cx - originX;
      let dy = cy - originY;
      let dist = Math.sqrt(dx*dx + dy*dy);
      
      if (dist > RADIUS) {
        dx = (dx / dist) * RADIUS;
        dy = (dy / dist) * RADIUS;
        dist = RADIUS;
      }
      
      knob.style.transform = `translate(calc(-50% + ${dx}px), calc(-50% + ${dy}px))`;
      
      // Map to values [-1.0, 1.0]
      jX = dx / RADIUS;
      jY = -(dy / RADIUS); // Invert Y so pushing up is forward
    }
    
    function resetKnob() {
      knob.style.transition = 'transform 0.2s cubic-bezier(0.175, 0.885, 0.32, 1.275)';
      knob.style.transform = 'translate(-50%, -50%)';
      setTimeout(() => knob.style.transition = 'none', 200);
      jX = 0; jY = 0;
      sendJoy(0, 0, 0);
    }
    
    function getOrigin() {
      const rect = base.getBoundingClientRect();
      originX = rect.left + rect.width / 2;
      originY = rect.top + rect.height / 2;
    }
    
    function sendJoy(x, y, m) {
      fetch(`/joy?x=${x.toFixed(3)}&y=${y.toFixed(3)}&m=${m.toFixed(3)}`).catch(() => {});
    }
    
    base.addEventListener('touchstart', e => {
      e.preventDefault();
      active = true;
      getOrigin();
      clearInterval(sendTimer);
      sendTimer = setInterval(() => {
        const mag = Math.min(Math.sqrt(jX*jX + jY*jY), 1.0);
        sendJoy(jX, jY, mag);
      }, 70);
    }, {passive: false});
    
    window.addEventListener('touchmove', e => {
      if (!active) return;
      e.preventDefault();
      processJoy(e.touches[0].clientX, e.touches[0].clientY);
    }, {passive: false});
    
    window.addEventListener('touchend', e => {
      if (!active) return;
      active = false;
      clearInterval(sendTimer);
      resetKnob();
    });
    
    // Mouse fallback for PC browser
    base.addEventListener('mousedown', e => {
      active = true;
      getOrigin();
      clearInterval(sendTimer);
      sendTimer = setInterval(() => {
        const mag = Math.min(Math.sqrt(jX*jX + jY*jY), 1.0);
        sendJoy(jX, jY, mag);
      }, 70);
    });
    window.addEventListener('mousemove', e => {
      if (active) processJoy(e.clientX, e.clientY);
    });
    window.addEventListener('mouseup', () => {
      if (active) {
        active = false;
        clearInterval(sendTimer);
        resetKnob();
      }
    });
    
    // Fetch telemetry data continuously
    function pollTelemetry() {
      fetch('/telemetry').then(r => r.json()).then(data => {
        // Update labels
        document.getElementById('poseDisplay').textContent = data.motion_state.toUpperCase();
        document.getElementById('statusBadge').textContent = data.motion_state.toUpperCase();
        document.getElementById('tMotionState').textContent = data.motion_state.toUpperCase();
        document.getElementById('tHeight').textContent = `LEVEL ${data.h_level} (${data.stand_z}mm)`;
        
        const xboxEl = document.getElementById('tXbox');
        if (data.xbox_connected) {
          xboxEl.textContent = "PAIRED & ACTIVE";
          xboxEl.className = "telemetry-value glowing";
        } else {
          xboxEl.textContent = "SEARCHING (BLE PAIRING MODE)";
          xboxEl.className = "telemetry-value";
          xboxEl.style.color = "var(--dim)";
        }
        
        // Pitch / Roll levelers
        document.getElementById('valPitch').textContent = `${data.pitch.toFixed(1)}°`;
        document.getElementById('valRoll').textContent = `${data.roll.toFixed(1)}°`;
        
        // Animate leveling gauge lines based on pitch/roll angles
        document.getElementById('linePitch').style.transform = `translateY(${-data.pitch * 1.5}px) rotate(${data.pitch * 0.8}deg)`;
        document.getElementById('lineRoll').style.transform = `translateY(${-data.roll * 1.5}px) rotate(${-data.roll * 0.8}deg)`;
      }).catch(err => console.error(err));
    }
    setInterval(pollTelemetry, 300);
    
    // Calibration loading
    function loadCalibration() {
      fetch('/calibration').then(r => r.json()).then(data => {
        const offsets = data.offsets;
        for(let l=0; l<4; l++) {
          const legDiv = document.querySelector(`.leg-section[data-leg="${l}"]`);
          const sliders = legDiv.querySelectorAll('.joint-slider');
          const values = legDiv.querySelectorAll('.joint-val');
          
          for(let j=0; j<3; j++) {
            sliders[j].value = offsets[l][j];
            values[j].textContent = `${offsets[l][j] >= 0 ? '+' : ''}${offsets[l][j]}°`;
          }
        }
      });
    }
    
    // Tuning joints live
    function tuneJoint(leg, joint, val) {
      const legDiv = document.querySelector(`.leg-section[data-leg="${leg}"]`);
      const valLabel = legDiv.querySelectorAll('.joint-val')[joint];
      valLabel.textContent = `${val >= 0 ? '+' : ''}${val}°`;
      
      // Fire GET command to adjust joint angle in memory immediately
      fetch(`/set_calib?leg=${leg}&joint=${joint}&offset=${val}`).catch(() => {});
    }
    
    // Persisting calibration to LittleFS flash memory
    function saveCalibration() {
      fetch('/save_calib').then(r => r.text()).then(txt => {
        alert("Success: Calibration persistent parameters written to LittleFS flash storage successfully!");
      }).catch(() => alert("Error: Failed to write configuration files to LittleFS flash partition."));
    }
  </script>
</body>
</html>
)rawHTML";

// --- HTTP SERVER HANDLERS ---
void handleRoot() {
  server.send_P(200, "text/html", HTML_PAGE);
}

void handleJoy() {
  if (server.hasArg("x") && server.hasArg("y") && server.hasArg("m")) {
    joyX   = constrain(server.arg("x").toFloat(), -1.0f, 1.0f);
    joyY   = constrain(server.arg("y").toFloat(), -1.0f, 1.0f);
    joyMag = constrain(server.arg("m").toFloat(),  0.0f, 1.0f);
  }
  server.send(200, "text/plain", "ok");
}

void handleCmd() {
  if (!server.hasArg("c")) {
    server.send(400, "text/plain", "missing param c");
    return;
  }
  
  char c = server.arg("c").charAt(0);
  switch (c) {
    case 's':
      joyX = joyY = joyMag = 0.0f;
      gaitRunning = false;
      poseStand(motionState == ST_SIT);
      break;
    case 'x':
      joyX = joyY = joyMag = 0.0f;
      poseSit();
      break;
    case 'u':
      if (hLevel < 4) {
        hLevel++;
        applyHeight();
      }
      break;
    case 'd':
      if (hLevel > 0) {
        hLevel--;
        applyHeight();
      }
      break;
    case 'g':
      joyX = joyY = joyMag = 0.0f;
      doGreeting();
      break;
  }
  server.send(200, "text/plain", "ok");
}

void handleTelemetry() {
  String stateStr = "stand";
  if (motionState == ST_SIT) stateStr = "sit";
  else if (motionState == ST_WALK) stateStr = "walk";
  else if (motionState == ST_GREET) stateStr = "greet";
  else if (motionState == ST_GREET_RETURN) stateStr = "greet return";

  String json = "{";
  json += "\"motion_state\":\"" + stateStr + "\"";
  json += ",\"h_level\":" + String(hLevel);
  json += ",\"stand_z\":" + String(HEIGHT_Z[hLevel]);
  json += ",\"xbox_connected\":" + String(xboxConnected ? "true" : "false");
  json += ",\"pitch\":" + String(bodyPitch, 2);
  json += ",\"roll\":" + String(bodyRoll, 2);
  json += "}";
  server.send(200, "application/json", json);
}

void handleGetCalib() {
  String json = "{\"offsets\":[";
  for (int i = 0; i < 4; i++) {
    json += "[";
    for (int j = 0; j < 3; j++) {
      json += String(calibration.offsets[i][j]);
      if (j < 2) json += ",";
    }
    json += "]";
    if (i < 3) json += ",";
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void handleSetCalib() {
  if (server.hasArg("leg") && server.hasArg("joint") && server.hasArg("offset")) {
    int leg = server.arg("leg").toInt();
    int joint = server.arg("joint").toInt();
    int offset = server.arg("offset").toInt();
    
    if (leg >= 0 && leg < 4 && joint >= 0 && joint < 3) {
      calibration.offsets[leg][joint] = offset;
      
      // Instantly rewrite the servo to apply calibration offsets on-the-fly
      writeServo(leg, joint);
    }
  }
  server.send(200, "text/plain", "ok");
}

void handleSaveCalib() {
  bool success = saveCalibration();
  server.send(200, "text/plain", success ? "ok" : "fail");
}

void handleResetCalib() {
  calibration.setDefaults();
  bool success = saveCalibration();
  server.send(200, "text/plain", success ? "Reset Successful. All offsets cleared. Please reboot ESP32." : "Reset Failed");
}

void initWebDashboard() {
  // Connect to House WiFi
  Serial.print(F("[WIFI] Connecting to House WiFi SSID: "));
  Serial.println(calibration.wifiSSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("rexa");
  WiFi.begin(calibration.wifiSSID, calibration.wifiPass);
  
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(F("[WIFI] Connected! IP address: "));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(F("[WIFI] Failed to connect to House WiFi."));
  }

  // Setup Web Server routing
  server.on("/", handleRoot);
  server.on("/joy", handleJoy);
  server.on("/cmd", handleCmd);
  server.on("/telemetry", handleTelemetry);
  server.on("/calibration", handleGetCalib);
  server.on("/set_calib", handleSetCalib);
  server.on("/save_calib", handleSaveCalib);
  server.on("/reset_calib", handleResetCalib);

  server.begin();
  Serial.println(F("[WEB] HTTP Server listening on port 80. Ready."));
}

void handleWebRequests() {
  server.handleClient();
}
