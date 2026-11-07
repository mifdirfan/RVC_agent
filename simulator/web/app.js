/* RVC System Test Simulator - UI.
 * Map editing, playback and inspection only. All RVC behaviour comes from the
 * trace produced by the real C controller (POST /run -> sim_runner). Nothing
 * here decides movement or cleaning, and no PASS/FAIL is assigned. */
'use strict';

const $ = (id) => document.getElementById(id);
const N = 10;
const TICK_LIMIT = 100;
const DIR_DEG = { N: 0, E: 90, S: 180, W: 270 };
const FSM_TEXT = {
  T1: 'Init: Enable Move Forward, Trigger Clean Normal',
  T2: 'FwdNormal, F && !L -> Turn Left', T3: 'FwdNormal, F && L && !R -> Turn Right',
  T4: 'FwdNormal, F && L && R -> Move Backward', T5: 'FwdNormal, stay (!F && !D)',
  T6: 'FwdNormal, !F && D -> Power-Up', T7: 'FwdPowerUp, F && !L -> Turn Left (cancel)',
  T8: 'FwdPowerUp, F && L && !R -> Turn Right (cancel)', T9: 'FwdPowerUp, F && L && R -> Backward (cancel)',
  T10: 'FwdPowerUp, stay (!F && !upDone)', T11: 'FwdPowerUp, upDone -> Clean Normal',
  T12: 'TurnLeft, stay (!turnDone)', T13: 'TurnLeft re-entry (turnDone, F, !L)', T14: 'TurnLeft -> TurnRight',
  T15: 'TurnLeft -> MoveBackward', T16: 'TurnLeft done, !F -> FwdNormal',
  T17: 'TurnRight, stay (!turnDone)', T18: 'TurnRight re-entry', T19: 'TurnRight -> TurnLeft',
  T20: 'TurnRight -> MoveBackward', T21: 'TurnRight done, !F -> FwdNormal',
  T22: 'MoveBackward, revMin && !L -> Turn Left', T23: 'MoveBackward, revMin && L && !R -> Turn Right',
  T24: 'MoveBackward, stay (!revMin)', T25: 'MoveBackward, stay (L && R)',
};

const S = {
  scenarios: [],
  evals: {},
  sel: null,          // selected scenario object (as loaded)
  map: null,          // editable map {start:{x,y,dir}, obstacles:Set, dust:Set, max_ticks}
  modified: false,    // map differs from the selected scenario
  trace: null,
  source: '',
  tick: 0,
  timer: null,
  editing: false,
  paint: null,        // active paint stroke {tool, add}
};

/* ---------- helpers ---------- */
const key = (x, y) => `${x},${y}`;
const unkey = (k) => k.split(',').map(Number);
function el(tag, attrs, ...kids) {
  const e = document.createElement(tag);
  if (attrs) for (const [k, v] of Object.entries(attrs)) {
    if (k === 'class') e.className = v; else if (k === 'text') e.textContent = v; else e.setAttribute(k, v);
  }
  for (const k of kids) if (k != null) e.append(k);
  return e;
}
function setMsg(id, text, kind) {
  const m = $(id);
  m.textContent = text || '';
  m.className = 'status' + (kind ? ' ' + kind : '');
}
function boolCell(v) {
  if (v === null || v === undefined) return el('span', { class: 'f', text: '–' });
  return el('span', { class: v ? 't' : 'f', text: v ? 'true' : 'false' });
}
function download(name, obj) {
  const blob = new Blob([JSON.stringify(obj, null, 2)], { type: 'application/json' });
  const a = el('a', { href: URL.createObjectURL(blob), download: name });
  document.body.append(a); a.click(); a.remove();
  setTimeout(() => URL.revokeObjectURL(a.href), 1000);
}
async function api(method, url, body) {
  const r = await fetch(url, {
    method, headers: body ? { 'Content-Type': 'application/json' } : undefined,
    body: body ? JSON.stringify(body) : undefined,
  });
  let data = null;
  try { data = await r.json(); } catch (e) { data = { error: 'invalid server response' }; }
  return { ok: r.ok, status: r.status, data };
}

/* ---------- map model ---------- */
function mapFromScenario(s) {
  return {
    start: { x: s.start.x, y: s.start.y, dir: s.start.dir },
    obstacles: new Set(s.obstacles.map(([x, y]) => key(x, y))),
    dust: new Set(s.dust.map(([x, y]) => key(x, y))),
    max_ticks: s.max_ticks,
  };
}
function sortedCells(set) {
  return [...set].map(unkey).sort((a, b) => a[1] - b[1] || a[0] - b[0]);
}
function mapToJson(m) {
  return { start: { ...m.start }, obstacles: sortedCells(m.obstacles), dust: sortedCells(m.dust), max_ticks: m.max_ticks };
}
function validateMap(m) {
  const errs = [];
  const { x, y, dir } = m.start;
  if (!(Number.isInteger(x) && Number.isInteger(y) && x >= 0 && x < N && y >= 0 && y < N)) errs.push('start must be inside the 10x10 grid');
  if (!['N', 'E', 'S', 'W'].includes(dir)) errs.push('heading must be N, E, S or W');
  if (m.obstacles.has(key(x, y))) errs.push(`start (${x},${y}) is on an obstacle`);
  for (const k of m.obstacles) if (m.dust.has(k)) errs.push(`cell (${k}) is both obstacle and dust`);
  for (const k of [...m.obstacles, ...m.dust]) {
    const [cx, cy] = unkey(k);
    if (!(cx >= 0 && cx < N && cy >= 0 && cy < N)) errs.push(`cell (${k}) is outside the grid`);
  }
  if (!(Number.isInteger(m.max_ticks) && m.max_ticks >= 1 && m.max_ticks <= TICK_LIMIT)) errs.push('max ticks must be 1..100');
  return errs;
}

/* ---------- grid ---------- */
const cells = [];
function buildGrid() {
  const g = $('grid');
  g.append(el('div', { class: 'ax' }));
  for (let x = 0; x < N; x++) g.append(el('div', { class: 'ax', text: String(x) }));
  for (let y = 0; y < N; y++) {
    g.append(el('div', { class: 'ax', text: String(y) }));
    cells[y] = [];
    for (let x = 0; x < N; x++) {
      const c = el('div', { class: 'cell', role: 'gridcell', 'data-x': x, 'data-y': y, title: `(${x},${y})` });
      c.addEventListener('pointerdown', (ev) => { ev.preventDefault(); onCellDown(x, y); });
      c.addEventListener('pointerenter', () => onCellEnter(x, y));
      g.append(c);
      cells[y][x] = c;
    }
  }
  window.addEventListener('pointerup', () => { S.paint = null; });
}

function snapshot() {
  return S.trace ? S.trace.ticks[S.tick] : null;
}

function renderGrid() {
  const snap = snapshot();
  const m = S.map;
  const dust = snap ? new Set(snap.dust.map(([x, y]) => key(x, y))) : m.dust;
  const pose = snap ? snap.pose : m.start;
  const visited = new Set();
  if (snap) for (let i = 0; i <= S.tick; i++) {
    const t = S.trace.ticks[i];
    visited.add(key(t.pose.x, t.pose.y));
    if (t.sensed_at) visited.add(key(t.sensed_at.x, t.sensed_at.y));
  }
  const coll = snap && snap.motion && snap.motion.collision;
  for (let y = 0; y < N; y++) for (let x = 0; x < N; x++) {
    const c = cells[y][x];
    const k = key(x, y);
    c.className = 'cell'
      + (m.obstacles.has(k) ? ' obs' : '')
      + (dust.has(k) ? ' dust' : '')
      + (visited.has(k) ? ' path' : '')
      + (S.editing && k === key(m.start.x, m.start.y) ? ' start-mark' : '');
    c.replaceChildren();
    if (x === pose.x && y === pose.y) {
      const cleaner = snap && snap.outputs ? snap.outputs.cleaner : null;
      const cls = cleaner === 'clean_up' ? 'c-up' : cleaner === 'clean_on' ? 'c-on' : 'c-off';
      const r = el('div', { class: `rvc ${cls}${coll ? ' collide' : ''}`, title: `RVC at (${x},${y}) heading ${pose.dir}` });
      const svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
      svg.setAttribute('viewBox', '0 0 10 10');
      svg.style.transform = `rotate(${DIR_DEG[pose.dir]}deg)`;
      const p = document.createElementNS('http://www.w3.org/2000/svg', 'path');
      p.setAttribute('d', 'M5 0.5 L9 9 L5 7 L1 9 Z');
      svg.append(p);
      r.append(svg);
      const mo = snap && snap.motion;
      if (mo && (mo.kind === 'turning_left' || mo.kind === 'turning_right')) {
        const done = mo.rotated ? 3 : mo.turn_progress;
        r.append(el('span', { class: 'turnbadge', text: `${mo.kind === 'turning_left' ? '↺' : '↻'} ${done}/3` }));
      }
      if (coll) r.append(el('span', { class: 'turnbadge', text: 'collision' }));
      c.append(r);
    }
  }
}

/* ---------- editing ---------- */
function onCellDown(x, y) {
  if (!S.editing) return;
  const tool = document.querySelector('input[name="tool"]:checked').value;
  const k = key(x, y);
  const m = S.map;
  let add = true;
  if (tool === 'obstacle') add = !m.obstacles.has(k);
  if (tool === 'dust') add = !m.dust.has(k);
  S.paint = { tool, add };
  applyTool(x, y);
}
function onCellEnter(x, y) {
  if (S.editing && S.paint && S.paint.tool !== 'start') applyTool(x, y);
}
function applyTool(x, y) {
  const { tool, add } = S.paint;
  const m = S.map;
  const k = key(x, y);
  const isStart = m.start.x === x && m.start.y === y;
  let changed = false;
  if (tool === 'obstacle') {
    if (add) {
      if (isStart) return refuse(x, y, 'Refused: an obstacle cannot be placed on the start cell.');
      if (m.dust.has(k)) return refuse(x, y, `Refused: (${x},${y}) holds dust. Erase it first.`);
      if (!m.obstacles.has(k)) { m.obstacles.add(k); changed = true; }
    } else if (m.obstacles.delete(k)) changed = true;
  } else if (tool === 'dust') {
    if (add) {
      if (m.obstacles.has(k)) return refuse(x, y, `Refused: (${x},${y}) is an obstacle. Erase it first.`);
      if (!m.dust.has(k)) { m.dust.add(k); changed = true; }
    } else if (m.dust.delete(k)) changed = true;
  } else if (tool === 'erase') {
    changed = m.obstacles.delete(k) | m.dust.delete(k);
  } else if (tool === 'start') {
    if (m.obstacles.has(k)) return refuse(x, y, `Refused: (${x},${y}) is an obstacle; the RVC cannot start there.`);
    if (!isStart) { m.start.x = x; m.start.y = y; changed = true; }
  }
  if (changed) { setMsg('mapMsg', ''); markModified(); }
}
function refuse(x, y, text) {
  setMsg('mapMsg', text, 'err');
  const c = cells[y][x];
  c.classList.remove('flash'); void c.offsetWidth; c.classList.add('flash');
}
function markModified() {
  S.modified = true;
  pause();
  S.trace = null;
  S.source = '';
  S.tick = 0;
  render();
  renderScenarioInfo();
}

/* ---------- scenarios ---------- */
async function loadScenarios(selectId) {
  const r = await api('GET', '/api/scenarios');
  if (!r.ok) { setMsg('mapMsg', 'Cannot load scenarios: ' + (r.data.error || r.status), 'err'); return; }
  S.scenarios = r.data;
  const sel = $('scenarioSelect');
  sel.replaceChildren();
  const gb = el('optgroup', { label: 'Built-in scenarios' });
  const gc = el('optgroup', { label: 'Custom scenarios' });
  for (const s of S.scenarios) (s.custom ? gc : gb).append(el('option', { value: s.id, text: `${s.id} – ${s.name}` }));
  sel.append(gb);
  if (gc.children.length) sel.append(gc);
  const known = selectId && S.scenarios.some((s) => s.id === selectId);
  const id = known ? selectId : (S.scenarios[0] && S.scenarios[0].id);
  if (id) { sel.value = id; await selectScenario(id); }
}
async function loadEvals() {
  const r = await api('GET', '/api/evaluations');
  S.evals = r.ok ? r.data : {};
}

async function selectScenario(id) {
  pause();
  S.sel = S.scenarios.find((s) => s.id === id);
  if (!S.sel) return;
  S.map = mapFromScenario(S.sel);
  S.modified = false;
  syncEditInputs();
  S.trace = null;
  S.tick = 0;
  if (S.sel.has_trace) {
    const r = await api('GET', `/traces/${encodeURIComponent(id)}.json`);
    if (r.ok) { S.trace = r.data; S.source = 'pre-generated'; }
  }
  if (!S.trace) await runController();
  buildHistory();
  renderScenarioInfo();
  renderEvalExisting();
  clearEvalForm();
  render();
}

function renderScenarioInfo() {
  const box = $('scenarioInfo');
  box.replaceChildren();
  if (!S.sel) return;
  const s = S.sel;
  const m = S.map;
  const dl = el('dl');
  const row = (k, v) => { dl.append(el('dt', { text: k }), el('dd', null, v)); };
  row('ID', document.createTextNode(S.modified ? `${s.id} (modified, unsaved)` : s.id + (s.custom ? ' (custom)' : '')));
  row('Name', document.createTextNode(s.name || ''));
  row('Start', document.createTextNode(`(${m.start.x},${m.start.y}) heading ${m.start.dir}` + (m.start.x === 4 && m.start.y === 4 ? ' – centre' : ' – custom start')));
  row('Obstacles', document.createTextNode(String(m.obstacles.size)));
  row('Dust', document.createTextNode(String(m.dust.size)));
  row('Max ticks', document.createTextNode(String(m.max_ticks)));
  if (s.requirements) {
    const c = el('div', { class: 'chips' });
    s.requirements.forEach((r) => c.append(el('span', { class: 'chip', text: r })));
    row('Requirements', c);
  }
  if (s.fsm) {
    const c = el('div', { class: 'chips' });
    s.fsm.forEach((r) => c.append(el('span', { class: 'chip', text: r, title: FSM_TEXT[r] || '' })));
    row('FSM rows', c);
  }
  if (s.ps) row('Processes', document.createTextNode(s.ps.join(', ')));
  if (s.key_ticks && s.key_ticks.length) {
    const c = el('div', { class: 'chips' });
    s.key_ticks.forEach((t) => {
      const ch = el('span', { class: 'chip tk', text: `tick ${t}`, title: 'Jump to tick' });
      ch.addEventListener('click', () => goTo(t));
      c.append(ch);
    });
    row('Key ticks', c);
  }
  box.append(dl);
  if (s.functions) {
    box.append(el('div', { class: 'exp-title', text: 'Essential functions under test' }));
    const ul = el('ul');
    (s.function_text || s.functions).forEach((f) => ul.append(el('li', { text: f })));
    box.append(ul);
  }
  if (s.expected && s.expected.length) {
    box.append(el('div', { class: 'exp-title', text: 'Expected behaviour (from requirements)' }));
    const ul = el('ul');
    s.expected.forEach((e) => ul.append(el('li', { text: e })));
    box.append(ul);
  }
  if (S.modified) box.append(el('p', { class: 'hint', text: 'The map was edited: the expected behaviour above belongs to the original scenario.' }));
}

/* ---------- execution ---------- */
async function runController() {
  const errs = validateMap(S.map);
  if (errs.length) { setMsg('mapMsg', 'Invalid map: ' + errs.join('; '), 'err'); return false; }
  pause();
  setMsg('mapMsg', 'Running C controller…');
  const r = await api('POST', '/run', mapToJson(S.map));
  if (!r.ok) { setMsg('mapMsg', 'Rejected by sim_runner: ' + (r.data.error || r.status), 'err'); return false; }
  S.trace = r.data;
  S.source = 'live run';
  S.tick = 0;
  setMsg('mapMsg', `C controller executed from reset: ${S.trace.ticks.length - 1} ticks recorded.`, 'ok');
  buildHistory();
  render();
  return true;
}
async function ensureTrace() {
  if (S.trace) return true;
  return runController();
}
function maxTick() { return S.trace ? S.trace.ticks.length - 1 : 0; }
function goTo(t) {
  if (!S.trace) return;
  S.tick = Math.max(0, Math.min(maxTick(), t));
  render();
}
async function start() {
  if (!(await ensureTrace())) return;
  if (S.tick >= maxTick()) S.tick = 0;
  pause();
  const ms = 1000 / Number($('speed').value);
  S.timer = setInterval(() => {
    if (S.tick >= maxTick()) { pause(); setMsg('mapMsg', `Stopped at the tick limit (${maxTick()}).`); return; }
    S.tick++;
    render();
  }, ms);
  render();
}
function pause() {
  if (S.timer) { clearInterval(S.timer); S.timer = null; }
  updateButtons();
}
async function step() {
  if (!(await ensureTrace())) return;
  pause();
  if (S.tick < maxTick()) S.tick++;
  else setMsg('mapMsg', `Tick limit reached (${maxTick()}).`);
  render();
}
function back() { pause(); if (S.tick > 0) S.tick--; render(); }
function reset() { pause(); S.tick = 0; render(); }
async function restart() { pause(); if (await runController()) start(); }

/* ---------- render ---------- */
function updateButtons() {
  const has = !!S.trace;
  $('pauseBtn').disabled = !S.timer;
  $('startBtn').disabled = !!S.timer;
  $('backBtn').disabled = !has || S.tick === 0;
  $('stepBtn').disabled = has && S.tick >= maxTick();
  $('resetBtn').disabled = !has;
  $('exportTraceBtn').disabled = !has;
}
function render() {
  $('tickNow').textContent = S.trace ? String(S.tick) : '–';
  $('tickMax').textContent = S.trace ? String(maxTick()) : String(S.map ? S.map.max_ticks : '–');
  const b = $('traceSource');
  if (!S.trace) { b.textContent = S.modified ? 'map edited – run needed' : 'no trace'; b.className = 'badge stale'; }
  else { b.textContent = S.source; b.className = 'badge ' + (S.source === 'live run' ? 'live' : 'pre'); }
  const sc = $('scrub');
  sc.max = String(maxTick());
  sc.value = String(S.tick);
  sc.disabled = !S.trace;
  $('grid').classList.toggle('editing', S.editing);
  renderGrid();
  renderInspector();
  markHistory();
  updateButtons();
}

function fmtPose(p) { return p ? `(${p.x},${p.y}) ${p.dir}` : '–'; }
function fmtList(a) { return a && a.length ? a.join(', ') : '—'; }

function inspectorRows(t) {
  if (!t) return null;
  const se = t.sensors || {};
  const c = t.controller;
  const mo = t.motion;
  return [
    ['Tick & pose', [
      ['Tick', t.tick],
      ['Phase', t.phase === 'startup' ? 'start-up (rvc_startup)' : 'tick (rvc_tick)'],
      ['Sensed at (position, heading)', fmtPose(t.sensed_at)],
      ['Position after tick (x, y)', t.pose ? `(${t.pose.x}, ${t.pose.y})` : '–'],
      ['Direction (heading) after tick', t.pose ? t.pose.dir : '–'],
      ['Turn progress (motor)', mo && mo.kind.startsWith('turning') ? `${mo.rotated ? 3 : mo.turn_progress}/3${mo.rotated ? ' – rotated 90°' : ''}` : '—'],
    ]],
    ['Sensor inputs (from map)', [
      ['Front sensor', t.sensors ? se.front : null],
      ['Left sensor', t.sensors ? se.left : null],
      ['Right sensor', t.sensors ? se.right : null],
      ['Dust sensor', t.sensors ? se.dust : null],
      ['Front interrupt raised', t.front_interrupt === null ? '—' : `front_sensor_on_interrupt(${t.front_interrupt})`],
      ['Port reads L / R / D', `${t.port_calls.read_left} / ${t.port_calls.read_right} / ${t.port_calls.read_dust}`],
    ]],
    ['Data stores (after sensing)', [
      ['Obstacle Location.front', t.stores.front],
      ['Obstacle Location.left', t.stores.left],
      ['Obstacle Location.right', t.stores.right],
      ['Dust Existence', t.stores.dust],
    ]],
    ['Controller (2.1.1 FSM)', [
      ['Previous state', t.transition.from || '—'],
      ['Current state', c.state],
      ['Transition (FSM row)', `${t.transition.fsm} – ${FSM_TEXT[t.transition.fsm] || ''}`],
      ['State transition this tick', t.transition.changed],
      ['Turn tick count (turnDone = 3)', c.turn_ticks],
      ['Reverse tick count (revMin ≥ 3)', c.reverse_ticks],
      ['Power-up tick count (upDone = 3)', c.power_up_ticks],
      ['Move Forward active (2.1.2)', c.move_forward_active],
      ['Move Backward active (2.1.5)', c.move_backward_active],
    ]],
    ['Outputs', [
      ['Direction written this tick', fmtList(t.port_calls.write_direction)],
      ['Clean written this tick', fmtList(t.port_calls.write_clean)],
      ['Motor output (held)', t.outputs.motor || '—'],
      ['Cleaner output (held)', t.outputs.cleaner || '—'],
    ]],
    ['Environment effect', [
      ['Motion', mo ? mo.kind : '—'],
      ['Collision (blocked move)', mo ? mo.collision : null],
      ['Dust removed from', t.cleaned ? `(${t.cleaned[0]},${t.cleaned[1]})` : '—'],
      ['Dust cells remaining', t.dust.length],
    ]],
  ];
}

function renderInspector() {
  const box = $('inspector');
  box.replaceChildren();
  const t = snapshot();
  if (!t) {
    box.append(el('p', { class: 'hint', text: 'No trace. Press "Run C controller" to execute the controller on this map.' }));
    return;
  }
  const cur = inspectorRows(t);
  const prev = S.tick > 0 ? inspectorRows(S.trace.ticks[S.tick - 1]) : null;
  cur.forEach(([title, rows], gi) => {
    const tb = el('table');
    rows.forEach(([label, v], ri) => {
      const pv = prev ? prev[gi][1][ri][1] : undefined;
      const changed = !!prev && label !== 'Tick' && label !== 'Phase' && JSON.stringify(pv) !== JSON.stringify(v);
      const vcell = el('td', { class: 'v' }, typeof v === 'boolean' || v === null ? boolCell(v) : document.createTextNode(String(v)));
      if (label === 'Collision (blocked move)' && v === true) vcell.classList.add('warn');
      if (changed) vcell.append(el('div', { class: 'prev', text: `prev: ${pv === null || pv === undefined ? '–' : pv}` }));
      tb.append(el('tr', changed ? { class: 'changed' } : null, el('td', { text: label }), vcell));
    });
    box.append(el('div', { class: 'insp-group' }, el('h3', { text: title }), tb));
  });
}

/* ---------- history ---------- */
function b(v) { return v === undefined || v === null ? '–' : v ? '1' : '0'; }
function buildHistory() {
  const tb = document.querySelector('#history tbody');
  tb.replaceChildren();
  if (!S.trace) return;
  const only = $('onlyChanges').checked;
  for (const t of S.trace.ticks) {
    const se = t.sensors || {};
    const c = t.controller;
    const wr = t.port_calls.write_direction.length || t.port_calls.write_clean.length;
    const coll = t.motion && t.motion.collision;
    if (only && !(t.transition.changed || wr || coll || t.tick === 0)) continue;
    const tr = el('tr', { 'data-tick': t.tick, class: (coll ? 'coll' : t.transition.changed ? 'tr' : '') });
    const cols = [
      t.tick, fmtPose(t.sensed_at), b(se.front), b(se.left), b(se.right), b(se.dust),
      t.front_interrupt === null ? '' : `irq(${t.front_interrupt ? 1 : 0})`,
      c.state, t.transition.fsm, c.turn_ticks, c.reverse_ticks, c.power_up_ticks,
      b(c.move_forward_active), b(c.move_backward_active),
      t.port_calls.write_direction.join(', '), t.port_calls.write_clean.join(', '),
      t.outputs.motor || '', t.outputs.cleaner || '',
      t.motion ? t.motion.kind + (coll ? ' (COLLISION)' : '') : '', fmtPose(t.pose),
      t.cleaned ? `(${t.cleaned.join(',')})` : '',
    ];
    cols.forEach((v, i) => tr.append(el('td', i === 8 ? { text: String(v), title: FSM_TEXT[v] || '' } : { text: String(v) })));
    tr.addEventListener('click', () => { pause(); goTo(t.tick); });
    tb.append(tr);
  }
}
function markHistory() {
  const rows = document.querySelectorAll('#history tbody tr');
  let cur = null;
  rows.forEach((r) => {
    const on = Number(r.dataset.tick) === S.tick;
    r.classList.toggle('cur', on);
    if (on) cur = r;
  });
  if (cur) {
    const wrap = cur.closest('.table-wrap');
    const top = cur.offsetTop - wrap.offsetTop;
    if (top < wrap.scrollTop + 24 || top > wrap.scrollTop + wrap.clientHeight - 24) wrap.scrollTop = top - wrap.clientHeight / 2;
  }
}

/* ---------- evaluation ---------- */
function clearEvalForm() {
  document.querySelectorAll('input[name="result"]').forEach((r) => { r.checked = false; });
  $('evalTicks').value = '';
  $('evalObs').value = '';
  setMsg('evalStatus', '');
}
function renderEvalExisting() {
  const box = $('evalExisting');
  box.replaceChildren();
  const e = S.sel && S.evals[S.sel.id];
  if (!e) { box.textContent = 'No evaluation recorded for this scenario yet (Pending).'; return; }
  box.append(el('div', { text: `Recorded: ${e.result} by ${e.evaluator || '(no name)'} at ${e.recorded_at}` }));
  if (e.related_ticks) box.append(el('div', { text: `Ticks: ${e.related_ticks}` }));
  if (e.observations) box.append(el('div', { text: `Observations: ${e.observations}` }));
  if (e.trace_file) box.append(el('div', { text: `Trace: ${e.trace_file}` }));
  if (e.history && e.history.length) box.append(el('div', { text: `${e.history.length} earlier evaluation(s) kept.` }));
}
async function saveEvaluation(ev) {
  ev.preventDefault();
  const res = document.querySelector('input[name="result"]:checked');
  if (!S.sel) return;
  if (S.modified) { setMsg('evalStatus', 'The map was edited. Save it as a scenario first, so the verdict refers to a saved map.', 'err'); return; }
  if (!S.trace) { setMsg('evalStatus', 'Run the C controller first: a verdict must refer to an executed trace.', 'err'); return; }
  if (!res) { setMsg('evalStatus', 'Choose PASS, FAIL or INCONCLUSIVE.', 'err'); return; }
  const record = {
    scenario_id: S.sel.id, scenario_name: S.sel.name, result: res.value,
    evaluator: $('evalEvaluator').value.trim(), observations: $('evalObs').value.trim(),
    related_ticks: $('evalTicks').value.trim(), trace_source: S.source, custom: !!S.sel.custom,
  };
  const r = await api('POST', '/api/evaluations', { record, trace: S.trace });
  if (!r.ok) { setMsg('evalStatus', 'Not saved: ' + (r.data.error || r.status), 'err'); return; }
  S.evals[S.sel.id] = r.data;
  try { localStorage.setItem('rvc.evaluator', record.evaluator); } catch (e) { /* storage unavailable */ }
  setMsg('evalStatus', `Saved (${r.data.result}). test_results.md updated; trace stored as ${r.data.trace_file}.`, 'ok');
  renderEvalExisting();
}

/* ---------- edit bar actions ---------- */
function syncEditInputs() {
  $('startDir').value = S.map.start.dir;
  $('maxTicks').value = S.map.max_ticks;
}
function toggleEdit() {
  S.editing = !S.editing;
  $('editToggle').setAttribute('aria-pressed', String(S.editing));
  $('editToggle').textContent = S.editing ? 'Done editing' : 'Edit map';
  $('editBar').hidden = !S.editing;
  if (S.editing) pause();
  render();
}
async function saveAsScenario(overwrite) {
  const errs = validateMap(S.map);
  if (errs.length) { setMsg('mapMsg', 'Invalid map: ' + errs.join('; '), 'err'); return; }
  const id = $('saveId').value.trim();
  if (!/^[A-Za-z0-9_-]{1,40}$/.test(id)) { setMsg('mapMsg', 'Enter an id of letters, digits, "-" or "_" (e.g. CU-01).', 'err'); return; }
  const body = {
    id, name: $('saveName').value.trim() || id, ...mapToJson(S.map), overwrite: !!overwrite,
    notes: S.sel ? `derived from ${S.sel.id}` : '',
  };
  const r = await api('POST', '/api/scenarios', body);
  if (r.status === 409 && !overwrite) {
    if (confirm(`Custom scenario ${id} exists. Overwrite it?`)) return saveAsScenario(true);
    return;
  }
  if (!r.ok) { setMsg('mapMsg', 'Not saved: ' + (r.data.error || r.status), 'err'); return; }
  setMsg('mapMsg', `Saved as ${r.data.saved}.`, 'ok');
  await loadScenarios(id);
}

/* ---------- wiring ---------- */
function wire() {
  $('scenarioSelect').addEventListener('change', (e) => selectScenario(e.target.value));
  $('runBtn').addEventListener('click', runController);
  $('startBtn').addEventListener('click', start);
  $('pauseBtn').addEventListener('click', pause);
  $('stepBtn').addEventListener('click', step);
  $('backBtn').addEventListener('click', back);
  $('resetBtn').addEventListener('click', reset);
  $('restartBtn').addEventListener('click', restart);
  $('speed').addEventListener('input', (e) => {
    $('speedVal').textContent = e.target.value;
    if (S.timer) start();
  });
  $('scrub').addEventListener('input', (e) => { pause(); goTo(Number(e.target.value)); });
  $('onlyChanges').addEventListener('change', () => { buildHistory(); markHistory(); });
  $('exportTraceBtn').addEventListener('click', () => {
    if (S.trace) download(`${S.sel ? S.sel.id : 'custom'}${S.modified ? '-modified' : ''}_trace.json`, S.trace);
  });

  $('editToggle').addEventListener('click', toggleEdit);
  $('startDir').addEventListener('change', (e) => { S.map.start.dir = e.target.value; markModified(); });
  $('maxTicks').addEventListener('change', (e) => {
    const v = Number(e.target.value);
    if (!(Number.isInteger(v) && v >= 1 && v <= TICK_LIMIT)) {
      setMsg('mapMsg', 'Max ticks must be an integer 1..100. Value not applied.', 'err');
      e.target.value = S.map.max_ticks;
      return;
    }
    S.map.max_ticks = v; markModified();
  });
  $('clearMapBtn').addEventListener('click', () => { S.map.obstacles.clear(); S.map.dust.clear(); markModified(); });
  $('resetMapBtn').addEventListener('click', () => { if (S.sel) selectScenario(S.sel.id); });
  $('centerBtn').addEventListener('click', () => {
    if (S.map.obstacles.has(key(4, 4))) { refuse(4, 4, 'Refused: (4,4) is an obstacle.'); return; }
    S.map.start.x = 4; S.map.start.y = 4; markModified();
  });
  $('validateBtn').addEventListener('click', () => {
    const errs = validateMap(S.map);
    setMsg('mapMsg', errs.length ? 'Invalid map: ' + errs.join('; ') : 'Map is valid.', errs.length ? 'err' : 'ok');
  });
  $('saveScenBtn').addEventListener('click', () => saveAsScenario(false));
  $('exportMapBtn').addEventListener('click', () => download(`${$('saveId').value.trim() || 'map'}.json`, mapToJson(S.map)));
  $('importMap').addEventListener('change', async (e) => {
    const f = e.target.files[0];
    e.target.value = '';
    if (!f) return;
    try {
      const j = JSON.parse(await f.text());
      if (!j.start || !Array.isArray(j.obstacles || []) || !Array.isArray(j.dust || [])) throw new Error('missing start/obstacles/dust');
      const m = mapFromScenario({ start: j.start, obstacles: j.obstacles || [], dust: j.dust || [], max_ticks: j.max_ticks || TICK_LIMIT });
      const errs = validateMap(m);
      if (errs.length) { setMsg('mapMsg', 'Imported map rejected: ' + errs.join('; '), 'err'); return; }
      S.map = m; syncEditInputs(); markModified();
      setMsg('mapMsg', `Imported ${f.name}.`, 'ok');
    } catch (err) {
      setMsg('mapMsg', 'Import failed: ' + err.message, 'err');
    }
  });

  $('evalForm').addEventListener('submit', saveEvaluation);
  $('addTickBtn').addEventListener('click', () => {
    if (!S.trace) return;
    const v = $('evalTicks').value.trim();
    $('evalTicks').value = v ? `${v}, ${S.tick}` : String(S.tick);
  });

  document.addEventListener('keydown', (e) => {
    if (e.target.closest('input, textarea, select')) return;
    if (e.key === 'ArrowRight') { e.preventDefault(); step(); }
    else if (e.key === 'ArrowLeft') { e.preventDefault(); back(); }
    else if (e.key === ' ') { e.preventDefault(); if (S.timer) pause(); else start(); }
  });
}

async function init() {
  buildGrid();
  wire();
  try { $('evalEvaluator').value = localStorage.getItem('rvc.evaluator') || ''; } catch (e) { /* storage unavailable */ }
  await loadEvals();
  // Deep link: #scenario=ST-11&tick=2 opens a scenario at a given tick.
  const h = new URLSearchParams(location.hash.slice(1));
  if (['light', 'dark'].includes(h.get('theme'))) document.documentElement.dataset.theme = h.get('theme');
  await loadScenarios(h.get('scenario') || undefined);
  if (h.has('tick')) goTo(Number(h.get('tick')));
  window.addEventListener('hashchange', openDeepLink);
}
async function openDeepLink() {
  const h = new URLSearchParams(location.hash.slice(1));
  const id = h.get('scenario');
  if (id && S.scenarios.some((s) => s.id === id) && (!S.sel || S.sel.id !== id || S.modified)) {
    $('scenarioSelect').value = id;
    await selectScenario(id);
  }
  if (h.has('tick')) { pause(); goTo(Number(h.get('tick'))); }
}
init();
