/* ═══════════════════════════════════════════════════
   IHM Supervision Industrielle — script.js
   Version corrigée pour backend JSON structuré :
   /api/state => { system, motor, gouverne, sensors, battery }
═══════════════════════════════════════════════════ */

const API_BASE = "http://localhost:8080/api";

document.addEventListener("DOMContentLoaded", () => {
  const $ = (id) => document.getElementById(id);

  /* ════════════════════════════════
     DOM
  ════════════════════════════════ */
  const navButtons = document.querySelectorAll(".nav-btn");
  const tabContents = document.querySelectorAll(".tab-content");
  const pageTitle = $("pageTitle");
  const pageSubtitle = $("pageSubtitle");

  const refreshBtn = $("refreshBtn");
  const pauseBtn = $("pauseBtn");
  const globalShutdownBtn = $("globalShutdownBtn");

  const sidebarBackendStatus = $("sidebarBackendStatus");

  // WebSocket
  const wsIpInput = $("wsIp");
  const wsPortInput = $("wsPort");
  const btnConnect = $("btnConnect");
  const btnDisconnect = $("btnDisconnect");
  const connectionStatus = $("connectionStatus");
  const errorMessage = $("errorMessage");

  // Dashboard
  const dashMotorBadge = $("dashMotorBadge");
  const dashMotorStatus = $("dashMotorStatus");
  const dashRpmActual = $("dashRpmActual");
  const dashRpmCmd = $("dashRpmCmd");
  const dashMotorTemp = $("dashMotorTemp");

  const dashLedEau = $("dashLedEau");
  const dashLedPression = $("dashLedPression");
  const dashLedTemp = $("dashLedTemp");
  const dashLedTemp1 = $("dashLedTemp1");
  const dashLedTemp2 = $("dashLedTemp2");
  const dashLedTemp3 = $("dashLedTemp3");
  const dashLedTempP = $("dashLedTempP");

  const dashBatBadge = $("dashBatBadge");
  const dashBatLevel = $("dashBatLevel");
  const dashBatTemp = $("dashBatTemp");
  const dashBatEtat = $("dashBatEtat");
  const dashBatAlarme = $("dashBatAlarme");

  // Moteur
  const sendRpmBtn = $("sendRpmBtn");
  const rpmInput = $("rpmInput");
  const motorStatusBadge = $("motorStatusBadge");
  const motorBannerDot = $("motorBannerDot");
  const motorStatus = $("motorStatus");
  const motorRpmActual = $("motorRpmActual");
  const motorRpmCmd = $("motorRpmCmd");
  const motorTemperature = $("motorTemperature");
  const motorCommandFeedback = $("motorCommandFeedback");
  const rpmGaugeFill = $("rpmGaugeFill");
  const rpmGaugePct = $("rpmGaugePct");
  const motorChartCanvas = $("motorChart");
  const mainChartCanvas = $("mainChart");

  // Capteurs
  const sensorEls = {
    eau: { led: $("ledEau"), retour: $("retourEau") },
    pression: { led: $("ledPression"), retour: $("retourPression") },
    temp1: { led: $("ledTemp1"), retour: $("retourTemp1") },
    temp2: { led: $("ledTemp2"), retour: $("retourTemp2") },
    temp3: { led: $("ledTemp3"), retour: $("retourTemp3") },
    tempPlaque: { led: $("ledTempPlaque"), retour: $("retourTempPlaque") }
  };

  // Batteries
  const batStatusBadge = $("batStatusBadge");
  const batTemp = $("batTemp");
  const batPctText = $("batPctText");
  const batBarFill = $("batBarFill");
  const batUmin = $("batUmin");
  const batUmax = $("batUmax");
  const batTmin = $("batTmin");
  const batTmax = $("batTmax");

  // détails batterie éventuels
  const batI = $("batI");
  const batU = $("batU");
  const batUCharge = $("batUCharge");
  const batUDech = $("batUDech");
  const batPower = $("batPower");
  const batCapacite = $("batCapacite");
  const batEnergie = $("batEnergie");
  const batBMSstate = $("batBMSstate");
  const batUelt1 = $("batUelt1");
  const batUelt2 = $("batUelt2");
  const batUelt3 = $("batUelt3");
  const batUelt4 = $("batUelt4");
  const batT1Disque = $("batT1Disque");
  const batT2Disque = $("batT2Disque");

  // Gouverne
  const gouvStatusBadge = $("gouvStatusBadge");
  const gouvBannerDot = $("gouvBannerDot");
  const gouvPosition = $("gouvPosition");
  const gouvVitesse = $("gouvVitesse");
  const gouvCouple = $("gouvCouple");
  const gouvStatutWord = $("gouvStatutWord");
  const gouvTxPDO = $("gouvTxPDO");
  const gouvNodeId = $("gouvNodeId");
  const gouvPosFill = $("gouvPosFill");
  const gouvPosPct = $("gouvPosPct");
  const gouvPosInput = $("gouvPosInput");
  const sendGouvPosBtn = $("sendGouvPosBtn");
  const gouvCommandFeedback = $("gouvCommandFeedback");

  // Système
  const systBackend = $("systBackend");
  const systUptime = $("systUptime");

  // Journaux
  const logList = $("logList");

  const batUelmin = $("batUelmin");
  const batUelmax = $("batUelmax");
  
  /* ════════════════════════════════
     NAVIGATION
  ════════════════════════════════ */
  const subtitles = {
    dashboard: "Supervision temps réel des équipements",
    moteur: "Commande et suivi du moteur principal",
    gouverne: "Positionneur — retour position, vitesse et couple",
    sensors: "Retour d'état des capteurs",
    batteries: "Surveillance énergétique et thermique",
    systeme: "Informations système et diagnostics"
  };

  const pageTitles = {
    dashboard: "Vue générale",
    moteur: "Moteur",
    gouverne: "Gouverne",
    sensors: "Capteurs",
    batteries: "Batteries",
    systeme: "Système"
  };

  navButtons.forEach((btn) => {
    btn.addEventListener("click", () => {
      navButtons.forEach((b) => b.classList.remove("active"));
      tabContents.forEach((s) => s.classList.remove("active"));

      btn.classList.add("active");
      const tab = btn.dataset.tab;
      const target = document.getElementById(tab);
      if (target) target.classList.add("active");

      if (pageTitle) pageTitle.textContent = pageTitles[tab] || tab;
      if (pageSubtitle) pageSubtitle.textContent = subtitles[tab] || "";
    });
  });

  /* ════════════════════════════════
     ÉTAT
  ════════════════════════════════ */
  let isPaused = false;
  let uptimeSeconds = 0;
  let ws = null;
  let isConnected = false;
  let wsPingInterval = null;

  const simState = {
    system: {
      server_status: "online",
      connection_status: "connected",
      mode: "supervision"
    },
    motor: {
      status: "RUN",
      rpm_actual: 1450,
      rpm_cmd: 1500,
      temperature: 39
    },
    gouverne: {
      position: 120.5,
      vitesse: 18,
      couple: 7,
      statut_word: 1,
      txpdo_ms: 50,
      node_id: 2,
      status: "CHECK"
    },
    sensors: {
      temperatures: {
        temp_smo1: 41,
        temp_smo2: 43,
        temp_smo3: 40,
        temp_plaque: 47
      },
      pressure: {
        value: 1.02,
        unit: "bar",
        state: 1,
        label: "Actif"
      },
      water: {
        state: 0,
        label: "Aucune détection",
        value: 0
      }
    },
    battery: {
      level: 78,
      temperature: 31,
      batt_T: 31,
      batt_i: 12.5,
      batt_u: 27.8,
      batt_uCharge: 29.4,
      batt_uDech: 22.0,
      batt_Power: 347.5,
      batt_Capacite: 42,
      batt_Energie: 1.18,
      uelmin: 3.62,
      uelmax: 3.79,
      Tmin: 24,
      Tmax: 31,
      etat: "Charge",
      alarme: "Aucune",
      BMSstate: "NORMAL",
      uelement1: 3.7,
      uelement2: 3.69,
      uelement3: 3.74,
      uelement4: 3.71
    }
  };

  /* ════════════════════════════════
     HELPERS
  ════════════════════════════════ */
  function setText(el, value, unit = "") {
  if (!el) return;
  el.textContent =
    value !== undefined && value !== null
      ? `${value}${unit}`
      : "–";
}

  function setLed(el, state) {
    if (!el) return;
    el.dataset.state = state;
  }

  function addLog(msg, level = "info") {
    if (!logList) return;
    const now = new Date();
    const ts = [now.getHours(), now.getMinutes(), now.getSeconds()]
      .map((n) => String(n).padStart(2, "0"))
      .join(":");

    const item = document.createElement("div");
    item.className = `log-item ${level}`;
    item.innerHTML = `<span class="log-time">${ts}</span><span class="log-text">${msg}</span>`;
    logList.prepend(item);

    while (logList.children.length > 10) {
      logList.removeChild(logList.lastChild);
    }
  }

  function showConnError(msg) {
    if (errorMessage) {
      errorMessage.textContent = msg;
      errorMessage.className = "conn-error-bar";
    }
    addLog(`Erreur : ${msg}`, "warn");
  }

  function clearConnError() {
    if (errorMessage) {
      errorMessage.textContent = "";
      errorMessage.className = "conn-error-bar hidden";
    }
  }

  function setConnectedUI(connected) {
    isConnected = connected;

    if (connectionStatus) {
      connectionStatus.textContent = connected ? "Connecté" : "Déconnecté";
      connectionStatus.className = connected ? "conn-status-bar success" : "conn-status-bar";
    }

    if (btnConnect) btnConnect.disabled = connected;
    if (btnDisconnect) btnDisconnect.disabled = !connected;

    if (sidebarBackendStatus) sidebarBackendStatus.textContent = connected ? "Connecté" : "–";
    if (systBackend) systBackend.textContent = connected ? "Connecté" : "–";

    addLog(
      connected ? "WebSocket connecté" : "WebSocket déconnecté",
      connected ? "info" : "warn"
    );
  }

  function validateAndBuildUrl() {
    const ip = (wsIpInput?.value || "").trim();
    const port = (wsPortInput?.value || "").trim();
    const ipPattern = /^(\d{1,3}\.){3}\d{1,3}$/;

    if (!ipPattern.test(ip)) return { error: "Adresse IP invalide" };
    if (ip.split(".").some((o) => parseInt(o, 10) > 255)) return { error: "Adresse IP invalide (octets > 255)" };
    if (!port || isNaN(parseInt(port, 10))) return { error: "Port invalide" };

    return { url: `ws://${ip}:${port}/` };
  }

  /* ════════════════════════════════
     HORLOGE / UPTIME
  ════════════════════════════════ */
  setInterval(() => {
    uptimeSeconds++;
    if (systUptime) {
      const h = String(Math.floor(uptimeSeconds / 3600)).padStart(2, "0");
      const m = String(Math.floor((uptimeSeconds % 3600) / 60)).padStart(2, "0");
      const s = String(uptimeSeconds % 60).padStart(2, "0");
      systUptime.textContent = `${h}:${m}:${s}`;
    }
  }, 1000);

  /* ════════════════════════════════
     PAUSE / SHUTDOWN
  ════════════════════════════════ */
  if (pauseBtn) {
    pauseBtn.addEventListener("click", () => {
      isPaused = !isPaused;
      pauseBtn.textContent = isPaused ? "▶ Reprendre" : "⏸ Pause";
      pauseBtn.classList.toggle("paused", isPaused);
      addLog(isPaused ? "Rafraîchissement mis en pause" : "Rafraîchissement repris", "info");
    });
  }

  function handleShutdown() {
    if (!window.confirm("Confirmer le shutdown complet du système ?")) return;
    addLog("⚠ Shutdown système demandé", "warn");
    window.alert("Shutdown simulé envoyé.");
  }

  if (globalShutdownBtn) {
    globalShutdownBtn.addEventListener("click", handleShutdown);
  }

  /* ════════════════════════════════
     MINI CHART MAISON
  ════════════════════════════════ */
  function IHMChart(canvas, cfg) {
    if (!canvas) return null;

    const PAD = { top: 28, right: 20, bottom: 44, left: 52 };
    const MAX_PTS = cfg.maxPoints || 12;
    const series = cfg.series.map((s) => ({
      label: s.label,
      color: s.color,
      fill: s.fill !== false,
      data: [...(s.data || [])],
      hidden: false
    }));
    let labels = [...(cfg.labels || [])];
    let tooltip = null;

    const ro = new ResizeObserver(() => draw());
    ro.observe(canvas);

    canvas.addEventListener("mousemove", onMove);
    canvas.addEventListener("mouseleave", () => {
      tooltip = null;
      draw();
    });

    function onMove(e) {
      const rect = canvas.getBoundingClientRect();
      const mx = e.clientX - rect.left;
      const plotW = (canvas.clientWidth || 400) - PAD.left - PAD.right;
      const n = labels.length;
      if (n < 2) return;

      let best = 0;
      let bestDx = Infinity;

      for (let i = 0; i < n; i++) {
        const dx = Math.abs(PAD.left + (i / (n - 1)) * plotW - mx);
        if (dx < bestDx) {
          bestDx = dx;
          best = i;
        }
      }

      tooltip = { idx: best };
      draw();
    }

    function draw() {
      const dpr = window.devicePixelRatio || 1;
      const W0 = canvas.clientWidth || 400;
      const H0 = canvas.clientHeight || 280;

      canvas.width = W0 * dpr;
      canvas.height = H0 * dpr;

      const ctx = canvas.getContext("2d");
      ctx.scale(dpr, dpr);

      const W = W0;
      const H = H0;
      ctx.clearRect(0, 0, W, H);

      const plotW = W - PAD.left - PAD.right;
      const plotH = H - PAD.top - PAD.bottom;
      const n = labels.length;
      if (n === 0) return;

      let yMin = Infinity;
      let yMax = -Infinity;

      series.forEach((s) => {
        if (s.hidden) return;
        s.data.forEach((v) => {
          if (v < yMin) yMin = v;
          if (v > yMax) yMax = v;
        });
      });

      if (yMin === Infinity) {
        yMin = 0;
        yMax = 100;
      }

      const range = yMax - yMin || 1;
      yMin -= range * 0.12;
      yMax += range * 0.12;

      const xAt = (i) => PAD.left + (n > 1 ? (i / (n - 1)) * plotW : 0.5 * plotW);
      const yAt = (v) => PAD.top + (1 - (v - yMin) / (yMax - yMin)) * plotH;

      for (let t = 0; t <= 5; t++) {
        const gy = yAt(yMin + (yMax - yMin) * (t / 5));
        ctx.strokeStyle = "rgba(42,140,255,0.07)";
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.moveTo(PAD.left, gy);
        ctx.lineTo(PAD.left + plotW, gy);
        ctx.stroke();

        ctx.fillStyle = "#4a6580";
        ctx.font = "11px Segoe UI,sans-serif";
        ctx.textAlign = "right";
        ctx.fillText(Math.round(yMin + (yMax - yMin) * (t / 5)), PAD.left - 6, gy + 4);
      }

      const showEvery = Math.max(1, Math.ceil(n / 8));
      for (let i = 0; i < n; i++) {
        const gx = xAt(i);
        ctx.strokeStyle = "rgba(42,140,255,0.06)";
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.moveTo(gx, PAD.top);
        ctx.lineTo(gx, PAD.top + plotH);
        ctx.stroke();

        if (i % showEvery === 0 || i === n - 1) {
          ctx.fillStyle = "#4a6580";
          ctx.font = "10px Segoe UI,sans-serif";
          ctx.textAlign = "center";
          ctx.fillText(labels[i] || "", gx, PAD.top + plotH + 16);
        }
      }

      ctx.strokeStyle = "rgba(42,140,255,0.15)";
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(PAD.left, PAD.top);
      ctx.lineTo(PAD.left, PAD.top + plotH);
      ctx.lineTo(PAD.left + plotW, PAD.top + plotH);
      ctx.stroke();

      series.forEach((s) => {
        if (s.hidden || s.data.length < 1) return;

        const pts = s.data.map((v, i) => [xAt(i), yAt(v)]);

        if (s.fill) {
          ctx.beginPath();
          ctx.moveTo(pts[0][0], PAD.top + plotH);
          pts.forEach(([x, y]) => ctx.lineTo(x, y));
          ctx.lineTo(pts[pts.length - 1][0], PAD.top + plotH);
          ctx.closePath();

          const m = s.color.match(/\d+/g);
          ctx.fillStyle = m
            ? `rgba(${m[0]},${m[1]},${m[2]},0.07)`
            : "rgba(42,140,255,0.07)";
          ctx.fill();
        }

        ctx.beginPath();
        ctx.strokeStyle = s.color;
        ctx.lineWidth = 2;
        ctx.lineJoin = "round";
        ctx.moveTo(pts[0][0], pts[0][1]);

        for (let i = 1; i < pts.length; i++) {
          const cpx = (pts[i - 1][0] + pts[i][0]) / 2;
          ctx.bezierCurveTo(cpx, pts[i - 1][1], cpx, pts[i][1], pts[i][0], pts[i][1]);
        }

        ctx.stroke();

        pts.forEach(([x, y]) => {
          ctx.beginPath();
          ctx.arc(x, y, 3, 0, Math.PI * 2);
          ctx.fillStyle = s.color;
          ctx.fill();
          ctx.strokeStyle = "rgba(2,13,26,0.8)";
          ctx.lineWidth = 1.5;
          ctx.stroke();
        });
      });

      let lx = PAD.left;
      series.forEach((s) => {
        ctx.globalAlpha = s.hidden ? 0.3 : 1;
        ctx.fillStyle = s.color;
        ctx.fillRect(lx, 10, 12, 3);
        ctx.fillStyle = "#6d8aab";
        ctx.font = "11px Segoe UI,sans-serif";
        ctx.textAlign = "left";
        ctx.fillText(s.label, lx + 16, 16);
        ctx.globalAlpha = 1;
        lx += ctx.measureText(s.label).width + 36;
      });

      if (tooltip !== null && tooltip.idx < n) {
        const i = tooltip.idx;
        const gx = xAt(i);

        ctx.strokeStyle = "rgba(42,140,255,0.3)";
        ctx.lineWidth = 1;
        ctx.setLineDash([4, 3]);
        ctx.beginPath();
        ctx.moveTo(gx, PAD.top);
        ctx.lineTo(gx, PAD.top + plotH);
        ctx.stroke();
        ctx.setLineDash([]);

        ctx.font = "11px Segoe UI,sans-serif";
        const lines = series
          .filter((s) => !s.hidden)
          .map((s) => `${s.label}: ${s.data[i] !== undefined ? Math.round(s.data[i] * 100) / 100 : "–"}`);

        const bw = Math.max(...lines.map((l) => ctx.measureText(l).width + 24), 100);
        const bh = lines.length * 18 + 22;

        let bx = gx + 10;
        let by = PAD.top;
        if (bx + bw > W - 10) bx = gx - bw - 10;

        ctx.fillStyle = "#061525";
        ctx.strokeStyle = "rgba(42,140,255,0.3)";
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.roundRect(bx, by, bw, bh, 6);
        ctx.fill();
        ctx.stroke();

        ctx.fillStyle = "#9aa7b8";
        ctx.textAlign = "left";
        ctx.fillText(labels[i] || `T${i}`, bx + 10, by + 14);

        series.filter((s) => !s.hidden).forEach((s, li) => {
          ctx.fillStyle = s.color;
          ctx.fillText(lines[li], bx + 10, by + 28 + li * 17);
        });

        series.filter((s) => !s.hidden).forEach((s) => {
          if (s.data[i] === undefined) return;
          ctx.beginPath();
          ctx.arc(gx, yAt(s.data[i]), 5, 0, Math.PI * 2);
          ctx.fillStyle = s.color;
          ctx.fill();
          ctx.strokeStyle = "#fff";
          ctx.lineWidth = 1.5;
          ctx.stroke();
        });
      }
    }

    draw();

    return {
      push(label, values) {
        labels.push(label);
        values.forEach((v, i) => {
          if (series[i]) series[i].data.push(v);
        });

        if (labels.length > MAX_PTS) {
          labels.shift();
          series.forEach((s) => s.data.shift());
        }

        draw();
      },
      toggleSeries(idx) {
        if (series[idx]) {
          series[idx].hidden = !series[idx].hidden;
          draw();
        }
      },
      isSeriesHidden(idx) {
        return series[idx]?.hidden === true;
      }
    };
  }

  /* ════════════════════════════════
     GRAPHES
  ════════════════════════════════ */
  const mainChart = IHMChart(mainChartCanvas, {
    maxPoints: 12,
    labels: ["T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8"],
    series: [
      { label: "RPM moteur", color: "#2a8cff", data: [900, 1100, 1250, 1380, 1450, 1490, 1430, 1450] },
      { label: "Température (°C)", color: "#f5a623", data: [34, 36, 37, 39, 40, 42, 41, 42] }
    ]
  });

  const motorChart = IHMChart(motorChartCanvas, {
    maxPoints: 12,
    labels: [],
    series: [
      { label: "RPM actuel", color: "#2a8cff", data: [] },
      { label: "Température (°C)", color: "#f5a623", data: [] }
    ]
  });

  function pushMotorChart(rpm, temp) {
    if (!motorChart) return;
    const now = new Date().toLocaleTimeString("fr-FR", {
      hour: "2-digit",
      minute: "2-digit",
      second: "2-digit"
    });
    motorChart.push(now, [rpm ?? 0, temp ?? 0]);
  }

  const chartRegistry = { mainChart, motorChart };

  document.querySelectorAll(".chart-toggle").forEach((btn) => {
    btn.addEventListener("click", () => {
      const idx = parseInt(btn.dataset.series, 10);
      const chart = chartRegistry[btn.dataset.chart];
      if (!chart) return;
      chart.toggleSeries(idx);
      btn.classList.toggle("active", !chart.isSeriesHidden(idx));
    });
  });

  /* ════════════════════════════════
     WEBSOCKET
  ════════════════════════════════ */
  function wsConnect() {
    clearConnError();
    if (ws && ws.readyState !== WebSocket.CLOSED) ws.close();

    const result = validateAndBuildUrl();
    if (result.error) {
      showConnError(result.error);
      return;
    }

    if (connectionStatus) {
      connectionStatus.textContent = "Connexion…";
      connectionStatus.className = "conn-status-bar";
    }
    if (btnConnect) btnConnect.disabled = true;

    addLog(`Tentative connexion → ${result.url}`, "info");

    try {
      ws = new WebSocket(result.url);
    } catch (e) {
      showConnError("Erreur de connexion : " + e.message);
      if (btnConnect) btnConnect.disabled = false;
      return;
    }

    ws.onopen = () => {
      setConnectedUI(true);
      wsPingInterval = setInterval(() => {
        if (ws && ws.readyState === WebSocket.OPEN) {
          try {
            ws.send(JSON.stringify({ type: "ping" }));
          } catch (_) {}
        }
      }, 5000);
    };

    ws.onerror = () => {
      showConnError("Erreur de connexion au serveur");
      setConnectedUI(false);
    };

    ws.onclose = (e) => {
      clearInterval(wsPingInterval);
      setConnectedUI(false);
      addLog(`WebSocket fermé (code ${e.code})`, "info");
    };

    ws.onmessage = (evt) => {
      try {
        const data = JSON.parse(evt.data);

        if (data.type === "pong") return;

        // cas 1 : état global complet
        if (
          data.system ||
          data.motor ||
          data.sensors ||
          data.battery ||
          data.gouverne
        ) {
          applyState(data);
          return;
        }

        // cas 2 : message spécialisé
        if (data.topic === "motor" && data.payload) renderMotor(data.payload);
        if (data.topic === "sensors" && data.payload) renderSensors(data.payload);
        if (data.topic === "battery" && data.payload) renderBattery(data.payload);
        if (data.topic === "gouverne" && data.payload) renderGouv(data.payload);
        if (data.topic === "system" && data.payload) renderStatus(data.payload);
      } catch (_) {
        addLog(`WS msg: ${evt.data.toString().substring(0, 60)}`, "info");
      }
    };
  }

  async function wsDisconnect() {
    if (!ws || ws.readyState !== WebSocket.OPEN) {
      showConnError("Aucune connexion active");
      return;
    }

    clearInterval(wsPingInterval);

    try {
      const ip = wsIpInput?.value.trim() || "127.0.0.1";
      const port = wsPortInput?.value.trim() || "5500";

      try {
        const res = await fetch(`http://${ip}:${port}/disconnect`);
        if (res.ok) {
          const d = await res.json();
          addLog(d.message || "Déconnexion OK", "info");
        }
      } catch (_) {
        addLog("Fermeture socket directe", "info");
      }

      ws.close();
      ws = null;
      setConnectedUI(false);
    } catch (e) {
      showConnError("Erreur déconnexion : " + e.message);
    }
  }

  if (btnConnect) btnConnect.addEventListener("click", wsConnect);
  if (btnDisconnect) btnDisconnect.addEventListener("click", wsDisconnect);

  /* ════════════════════════════════
     RENDER STATUS
  ════════════════════════════════ */
  function renderStatus(system) {
    const serverStatus = system?.server_status ?? simState.system.server_status;
    if (sidebarBackendStatus) sidebarBackendStatus.textContent = serverStatus;
    if (systBackend) systBackend.textContent = serverStatus;
  }

  /* ════════════════════════════════
     RENDER MOTEUR
  ════════════════════════════════ */
  function renderMotor(motor) {
    const d = motor || simState.motor;

    const rpm = d.rpm_actual ?? simState.motor.rpm_actual;
    const rpmCmd = d.rpm_cmd ?? simState.motor.rpm_cmd;
    const temp = d.temperature ?? simState.motor.temperature;
    const status = d.status ?? simState.motor.status;

    setText(motorStatus, status);
    setText(motorRpmActual, `${rpm} tr/min`);
    setText(motorRpmCmd, `${rpmCmd} tr/min`);
    setText(motorTemperature, `${temp} °C`);

    const pct = Math.round(Math.min(100, (rpm / 3000) * 100));
    if (rpmGaugeFill) rpmGaugeFill.style.width = `${pct}%`;
    if (rpmGaugePct) rpmGaugePct.textContent = `${pct}%`;

    const cls = status === "RUN" ? "ok" : status === "STOP" ? "warn" : "danger";
    if (motorStatusBadge) {
      motorStatusBadge.textContent = status;
      motorStatusBadge.className = `status-pill ${cls}`;
    }
    if (motorBannerDot) {
      motorBannerDot.className = `motor-banner-dot${status === "RUN" ? "" : " stop"}`;
    }

    setText(dashMotorStatus, status);
    setText(dashRpmActual, `${rpm} tr/min`);
    setText(dashRpmCmd, `${rpmCmd} tr/min`);
    setText(dashMotorTemp, `${temp} °C`);
    if (dashMotorBadge) {
      dashMotorBadge.textContent = status;
      dashMotorBadge.className = `status-pill ${cls}`;
    }

    pushMotorChart(rpm, temp);
  }

  /* ════════════════════════════════
     RENDER CAPTEURS
  ════════════════════════════════ */
  function renderSensors(sensors) {
    const d = sensors || simState.sensors;

    const temps = d.temperatures || simState.sensors.temperatures;
    const pressure = d.pressure || simState.sensors.pressure;
    const water = d.water || simState.sensors.water;

    const t1 = temps.temp_smo1 ?? simState.sensors.temperatures.temp_smo1;
    const t2 = temps.temp_smo2 ?? simState.sensors.temperatures.temp_smo2;
    const t3 = temps.temp_smo3 ?? simState.sensors.temperatures.temp_smo3;
    const tP = temps.temp_plaque ?? simState.sensors.temperatures.temp_plaque;

    const pressureValue = pressure.value ?? simState.sensors.pressure.value;
    const waterValue = water.value ?? simState.sensors.water.value;
    const waterState = water.state ?? simState.sensors.water.state;

    const tempState = (v) => (v < 50 ? "ok" : v < 80 ? "warn" : "error");

    setLed(sensorEls.temp1.led, tempState(t1));
    setText(sensorEls.temp1.retour, `${t1} °C`);

    setLed(sensorEls.temp2.led, tempState(t2));
    setText(sensorEls.temp2.retour, `${t2} °C`);

    setLed(sensorEls.temp3.led, tempState(t3));
    setText(sensorEls.temp3.retour, `${t3} °C`);

    setLed(sensorEls.tempPlaque.led, tempState(tP));
    setText(sensorEls.tempPlaque.retour, `${tP} °C`);

    const pressureState = pressure.state ?? simState.sensors.pressure.state;
    setLed(sensorEls.pression.led, pressureState === 1 ? "ok" : "warn");
    setText(sensorEls.pression.retour, `${pressureValue} ${pressure.unit ?? "bar"}`);

    setLed(sensorEls.eau.led, waterState === 0 ? "ok" : "error");
    setText(sensorEls.eau.retour, water.label ?? (waterState === 0 ? "Aucune détection" : "EAU DÉTECTÉE"));

    if (dashLedEau) dashLedEau.dataset.state = waterState === 0 ? "ok" : "error";
    if (dashLedPression) dashLedPression.dataset.state = pressureState === 1 ? "ok" : "warn";

    if (dashLedTemp1) dashLedTemp1.dataset.state = tempState(t1);
    if (dashLedTemp2) dashLedTemp2.dataset.state = tempState(t2);
    if (dashLedTemp3) dashLedTemp3.dataset.state = tempState(t3);
    if (dashLedTempP) dashLedTempP.dataset.state = tempState(tP);

    if (dashLedTemp) {
      const avg = (t1 + t2 + t3 + tP) / 4;
      dashLedTemp.dataset.state = tempState(avg);
    }
  }

  /* ════════════════════════════════
     RENDER BATTERIE
  ════════════════════════════════ */
 function renderBattery(battery) {
  const d = battery || simState.battery;

  const level = Math.round(d.level ?? simState.battery.level);
  const pct = `${level}%`;
  const temp = d.batt_T ?? d.temperature ?? simState.battery.temperature;

  if (batPctText) batPctText.textContent = pct;

  if (batBarFill) {
    batBarFill.style.width = pct;
    batBarFill.className =
      `bat-bar-fill${level < 20 ? " low" : level < 40 ? " warn" : ""}`;
  }

  // état BMS
  setText(batI, d.batt_i, " A");
  setText(batU, d.batt_u, " V");
  setText(batUCharge, d.batt_uCharge, " V");
  setText(batUDech, d.batt_uDech, " V");
  setText(batTemp, temp, " °C");
  setText(batPower, d.batt_Power, " W");
  setText(batCapacite, d.batt_Capacite, " Ah");
  setText(batEnergie, d.batt_Energie, " Wh");

  // info + tensions éléments
  setText(batUelmin, d.uelmin, " V");
  setText(batUelmax, d.uelmax, " V");
  setText(batTmin, d.Tmin, " °C");
  setText(batTmax, d.Tmax, " °C");
  setText(batBMSstate, d.BMSstate);

  setText(batUelt1, d.uelement1, " V");
  setText(batUelt2, d.uelement2, " V");
  setText(batUelt3, d.uelement3, " V");
  setText(batUelt4, d.uelement4, " V");

  // températures disques
  setText(batT1Disque, d.t1_disque, " °C");
  setText(batT2Disque, d.t2_disque, " °C");

  // bloc config du bas si présent
  setText(batUmin, d.uelmin, " V");
  setText(batUmax, d.uelmax, " V");

  // dashboard
  setText(dashBatLevel, pct);
  setText(dashBatTemp, temp, "°C");
  setText(dashBatEtat, d.etat);

  if (dashBatAlarme) {
    dashBatAlarme.textContent = d.alarme ?? "Aucune";
    dashBatAlarme.className =
      d.alarme && d.alarme !== "Aucune" ? "warn-text" : "success-text";
  }

  const batOk = level > 20 && temp < 45;

  if (batStatusBadge) {
    batStatusBadge.textContent = batOk ? "OK" : "MONITOR";
    batStatusBadge.className = `status-pill ${batOk ? "ok" : "warn"}`;
  }

  if (dashBatBadge) {
    dashBatBadge.textContent = batOk ? "OK" : "MONITOR";
    dashBatBadge.className = `status-pill ${batOk ? "ok" : "warn"}`;
  }
}

  function applyState(state) {
    if (!state) return;

    if (state.system) renderStatus(state.system);
    if (state.motor) renderMotor(state.motor);
    if (state.sensors) renderSensors(state.sensors);
    if (state.battery) renderBattery(state.battery);
    if (state.gouverne) renderGouv(state.gouverne);
  }

  /* ════════════════════════════════
     RENDER GOUVERNE
  ════════════════════════════════ */
  function renderGouv(gouverne) {
    const d = gouverne || simState.gouverne;

    const pos = d.position ?? simState.gouverne.position;
    const vit = d.vitesse ?? simState.gouverne.vitesse;
    const cpl = d.couple ?? simState.gouverne.couple;
    const sw = d.statut_word ?? simState.gouverne.statut_word;
    const txp = d.txpdo_ms ?? simState.gouverne.txpdo_ms;
    const nid = d.node_id ?? simState.gouverne.node_id;
    const status = d.status ?? simState.gouverne.status;

    setText(gouvPosition, pos.toFixed(1));
    setText(gouvVitesse, vit);
    setText(gouvCouple, cpl);
    setText(gouvStatutWord, sw);
    setText(gouvTxPDO, `${txp} ms`);
    setText(gouvNodeId, nid);

    const pct = Math.round(((pos + 150) / 300) * 100);
    if (gouvPosFill) gouvPosFill.style.width = `${Math.max(0, Math.min(100, pct))}%`;
    if (gouvPosPct) gouvPosPct.textContent = pos.toFixed(1);

    const cls = (status === "CHECK") ? "warn" : "ok";
    if (gouvStatusBadge) {
      gouvStatusBadge.textContent = status;
      gouvStatusBadge.className = `status-pill ${cls}`;
    }
    if (gouvBannerDot) {
      gouvBannerDot.className = `motor-banner-dot${status === "CHECK" ? " stop" : ""}`;
    }
  }

  /* ════════════════════════════════
     API
  ════════════════════════════════ */
  async function safeFetchState() {
  try {
    const r = await fetch(`${API_BASE}/state`);
    if (!r.ok) throw new Error("HTTP error");
    return await r.json();
  } catch (_) {
    return null;
  }
}

async function refreshAll() {
  if (isPaused) return;

  const state = await safeFetchState();

  if (!state) {
    applyState(simState);
    return;
  }

  applyState(state);
}

  /* ════════════════════════════════
     COMMANDES
  ════════════════════════════════ */
  async function sendMotorRpm() {
    if (!rpmInput) return;

    const val = parseInt(rpmInput.value, 10);
    if (isNaN(val) || val < 0 || val > 3000) {
      if (motorCommandFeedback) {
        motorCommandFeedback.textContent = "⚠ Valeur RPM invalide (0–3000).";
      }
      return;
    }

    try {
      const r = await fetch(`${API_BASE}/motor/rpm_cmd`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ rpm_cmd: val })
      });

      const d = await r.json();

      if (!r.ok || d.ok === false) {
        if (motorCommandFeedback) {
          motorCommandFeedback.textContent = d.message || "⚠ Erreur.";
        }
        return;
      }
    } catch (_) {}

    if (motorCommandFeedback) {
      motorCommandFeedback.textContent = `✓ Consigne envoyée : ${val} tr/min`;
    }

    addLog(`Consigne RPM : ${val} tr/min`, "info");
    await refreshAll();
  }

  async function sendGouvPosition() {
    if (!gouvPosInput) return;

    const val = parseFloat(gouvPosInput.value);
    if (isNaN(val) || val < -150 || val > 150) {
      if (gouvCommandFeedback) {
        gouvCommandFeedback.textContent = "⚠ Position invalide (−150 à +150).";
      }
      return;
    }

    try {
      const r = await fetch(`${API_BASE}/gouverne/position_cmd`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ position_cmd: val })
      });

      const d = await r.json();

      if (!r.ok || d.ok === false) {
        if (gouvCommandFeedback) {
          gouvCommandFeedback.textContent = d.message || "⚠ Erreur.";
        }
        return;
      }
    } catch (_) {
      if (gouvCommandFeedback) {
        gouvCommandFeedback.textContent = "⚠ Route position_cmd indisponible.";
      }
      return;
    }

    if (gouvCommandFeedback) {
      gouvCommandFeedback.textContent = `✓ Consigne envoyée : ${val}`;
    }

    addLog(`Gouverne — position : ${val}`, "info");
    await refreshAll();
  }

  if (sendRpmBtn) sendRpmBtn.addEventListener("click", sendMotorRpm);
  if (sendGouvPosBtn) sendGouvPosBtn.addEventListener("click", sendGouvPosition);
  if (refreshBtn) {
    refreshBtn.addEventListener("click", () => {
      refreshAll();
      addLog("Actualisation manuelle", "info");
    });
  }

  /* ════════════════════════════════
     INIT
  ════════════════════════════════ */
  renderStatus(simState.system);
  renderMotor(simState.motor);
  renderSensors(simState.sensors);
  renderBattery(simState.battery);
  renderGouv(simState.gouverne);

  refreshAll();
  setInterval(refreshAll, 1000);

  addLog("Interface IHM démarrée", "info");
  addLog("Connexion backend initialisée", "info");
  addLog("SMO en fonctionnement nominal", "info");
  addLog("Batterie sous surveillance thermique", "warn");
});