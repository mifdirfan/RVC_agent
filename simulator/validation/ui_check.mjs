// UI check for the RVC simulator (skill Step 10). Drives headless Chrome via the
// DevTools protocol and exercises the controls, map editing, history navigation
// and evaluation recording. It checks the SIMULATOR UI only, never the RVC.
//
// Prerequisites: the server is running (python simulator/server.py), Chrome is
// installed, Node >= 21. Usage:
//   node --experimental-websocket simulator/validation/ui_check.mjs [http://127.0.0.1:8000]
// Leaves no test data behind: the temporary custom scenario and evaluation are removed.
import { spawn, execFileSync } from 'node:child_process';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const BASE = process.argv[2] || 'http://127.0.0.1:8000';
const HERE = path.dirname(fileURLToPath(import.meta.url));
const SIM = path.dirname(HERE);
const CHROME = [
  'C:/Program Files/Google/Chrome/Application/chrome.exe',
  'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',
  '/usr/bin/google-chrome', '/usr/bin/chromium',
].find((p) => fs.existsSync(p));
if (!CHROME) { console.error('Chrome/Edge not found'); process.exit(2); }

const TEST_ID = 'ZZ-UITEST';
const evalFile = path.join(SIM, 'results', 'evaluations.json');
const evalBackup = fs.existsSync(evalFile) ? fs.readFileSync(evalFile, 'utf8') : null;
const resultsDirExisted = fs.existsSync(path.join(SIM, 'results'));
const customDirExisted = fs.existsSync(path.join(SIM, 'scenarios', 'custom'));

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
const profile = fs.mkdtempSync(path.join(os.tmpdir(), 'rvc-ui-'));
const port = 9333;
const chrome = spawn(CHROME, ['--headless=new', '--disable-gpu', '--no-first-run', `--user-data-dir=${profile}`,
  `--remote-debugging-port=${port}`, '--window-size=1400,1000', 'about:blank'], { stdio: 'ignore' });

let ws; let seq = 0; const pending = new Map();
function send(method, params = {}) {
  const id = ++seq;
  ws.send(JSON.stringify({ id, method, params }));
  return new Promise((res, rej) => pending.set(id, { res, rej }));
}
async function ev(expr) {
  const r = await send('Runtime.evaluate', { expression: expr, awaitPromise: true, returnByValue: true });
  if (r.exceptionDetails) throw new Error('page error: ' + (r.exceptionDetails.exception?.description || r.exceptionDetails.text));
  return r.result.value;
}
async function waitFor(expr, ms = 8000) {
  const t0 = Date.now();
  while (Date.now() - t0 < ms) { if (await ev(expr)) return true; await sleep(100); }
  return false;
}
const click = (id) => ev(`document.getElementById(${JSON.stringify(id)}).click()`);
const cellDown = (x, y) => ev(`(() => { const c = document.querySelector('.cell[data-x="${x}"][data-y="${y}"]');
  c.dispatchEvent(new PointerEvent('pointerdown', { bubbles: true })); window.dispatchEvent(new PointerEvent('pointerup')); })()`);
const tool = (t) => ev(`document.querySelector('input[name="tool"][value="${t}"]').click()`);

const results = [];
function check(id, title, ok, detail = '') { results.push({ id, title, ok: !!ok, detail }); console.log(`${ok ? 'OK     ' : 'PROBLEM'} ${id} ${title}${detail ? ' – ' + detail : ''}`); }

async function main() {
  let target;
  for (let i = 0; i < 50 && !target; i++) {
    try { target = (await (await fetch(`http://127.0.0.1:${port}/json`)).json()).find((t) => t.type === 'page'); } catch (e) { await sleep(200); }
  }
  ws = new WebSocket(target.webSocketDebuggerUrl);
  await new Promise((r) => { ws.onopen = r; });
  ws.onmessage = (m) => { const d = JSON.parse(m.data); if (d.id && pending.has(d.id)) { pending.get(d.id).res(d.result || d); pending.delete(d.id); } };
  await send('Page.enable');
  await send('Runtime.enable');
  const errors = [];
  ws.addEventListener('message', (m) => { const d = JSON.parse(m.data); if (d.method === 'Runtime.exceptionThrown') errors.push(d.params.exceptionDetails.text); });

  await send('Page.navigate', { url: BASE + '/' });
  const loaded = await waitFor('typeof S !== "undefined" && S.trace && S.sel && S.sel.id === "ST-01"');
  const nOpt = await ev('document.querySelectorAll("#scenarioSelect option").length');
  const nCells = await ev('document.querySelectorAll(".cell").length');
  check('U1', 'Page loads: scenario list, 10x10 grid, ST-01 trace', loaded && nOpt >= 22 && nCells === 100, `${nOpt} scenarios, ${nCells} cells`);

  // controls
  for (let i = 0; i < 3; i++) await click('stepBtn');
  const t3 = await ev('[S.tick, document.getElementById("tickNow").textContent, document.querySelector("#history tr.cur")?.dataset.tick]');
  check('U2', 'Step advances one tick at a time (tick counter, history row follow)', t3[0] === 3 && t3[1] === '3' && t3[2] === '3', JSON.stringify(t3));
  await click('backBtn');
  const tb = await ev('S.tick');
  await click('resetBtn');
  const tr = await ev('S.tick');
  check('U3', 'Step back inspects the previous snapshot; Reset returns to tick 0', tb === 2 && tr === 0, `back->${tb}, reset->${tr}`);

  // inspector / grid show the trace values
  let mism = [];
  for (const k of [0, 5, 6, 7, 8, 20, 50, 100]) {
    await ev(`goTo(${k})`);
    const v = await ev(`(() => { const t = S.trace.ticks[${k}];
      const rows = [...document.querySelectorAll('#inspector tr')].map(r => [r.children[0].textContent, r.children[1].firstChild.textContent]);
      const get = (l) => (rows.find(r => r[0] === l) || [])[1];
      const rvc = document.querySelector('.rvc').parentElement;
      return { state: get('Current state') === t.controller.state, motor: get('Motor output (held)') === (t.outputs.motor || '—'),
               pos: +rvc.dataset.x === t.pose.x && +rvc.dataset.y === t.pose.y }; })()`);
    if (!v.state || !v.motor || !v.pos) mism.push(`tick ${k}: ${JSON.stringify(v)}`);
  }
  check('U4', 'Displayed controller state, motor output and RVC cell equal the C trace (8 ticks of ST-01)', mism.length === 0, mism.join('; '));

  // auto play to the limit (ST-02 has 20 ticks)
  await ev('document.getElementById("scenarioSelect").value = "ST-02"; document.getElementById("scenarioSelect").dispatchEvent(new Event("change"))');
  await waitFor('S.sel.id === "ST-02" && S.trace');
  await ev('document.getElementById("speed").value = 20');
  await click('startBtn');
  const stopped = await waitFor('S.tick === 20 && S.timer === null', 6000);
  const msg = await ev('document.getElementById("mapMsg").textContent');
  check('U5', 'Start plays automatically and stops at the tick limit', stopped && /limit/.test(msg), msg);

  // history navigation
  await ev('document.querySelector(\'#history tr[data-tick="5"]\').click()');
  const h5 = await ev('S.tick');
  await ev('(() => { const s = document.getElementById("scrub"); s.value = 7; s.dispatchEvent(new Event("input")); })()');
  const s7 = await ev('S.tick');
  check('U6', 'History row click and tick slider navigate the saved history', h5 === 5 && s7 === 7, `row->${h5}, slider->${s7}`);

  // editing
  await click('editToggle');
  await tool('obstacle');
  await cellDown(4, 4);
  const refusedStart = await ev('!S.map.obstacles.has("4,4") && /Refused/.test(document.getElementById("mapMsg").textContent)');
  await cellDown(2, 2);
  const added = await ev('S.map.obstacles.has("2,2") && S.modified && S.trace === null && /edited/.test(document.getElementById("traceSource").textContent)');
  await tool('dust');
  await cellDown(2, 2);
  const refusedDust = await ev('!S.map.dust.has("2,2") && /Refused/.test(document.getElementById("mapMsg").textContent)');
  await cellDown(6, 6);
  await tool('start');
  await cellDown(1, 1);
  await ev('(() => { const d = document.getElementById("startDir"); d.value = "E"; d.dispatchEvent(new Event("change")); })()');
  check('U7', 'Map editing: obstacle/dust toggling, refusals on start cell and overlaps, start cell and heading change',
    refusedStart && added && refusedDust && (await ev('S.map.dust.has("6,6") && S.map.start.x === 1 && S.map.start.y === 1 && S.map.start.dir === "E"')),
    JSON.stringify({ refusedStart, added, refusedDust }));
  await ev('(() => { const m = document.getElementById("maxTicks"); m.value = 150; m.dispatchEvent(new Event("change")); })()');
  const maxRejected = await ev('S.map.max_ticks !== 150 && /1..100/.test(document.getElementById("mapMsg").textContent)');
  await ev('(() => { const m = document.getElementById("maxTicks"); m.value = 30; m.dispatchEvent(new Event("change")); })()');
  check('U8', 'Max ticks above 100 rejected; 30 accepted', maxRejected && (await ev('S.map.max_ticks === 30')));

  await click('runBtn');
  await waitFor('S.trace && S.source === "live run"');
  const live = await ev(`(() => { const t = S.trace; return { pose: t.ticks[0].pose, n: t.ticks.length,
    obs: JSON.stringify(t.scenario.obstacles), dust: JSON.stringify(t.scenario.dust) }; })()`);
  check('U9', 'Custom map executes the C controller from reset with the custom start', live.pose.x === 1 && live.pose.y === 1 &&
    live.pose.dir === 'E' && live.n === 31 && live.obs.includes('[2,2]') && live.dust.includes('[6,6]'), JSON.stringify(live));

  // evaluation needs a saved map
  await ev('document.querySelector(\'input[name="result"][value="INCONCLUSIVE"]\').click()');
  await click('saveEvalBtn');
  const blocked = await waitFor('/Save it as a scenario first/.test(document.getElementById("evalStatus").textContent)', 2000);
  await ev(`document.getElementById("saveId").value = "${TEST_ID}"; document.getElementById("saveName").value = "UI check map"`);
  await click('saveScenBtn');
  const saved = await waitFor(`S.sel && S.sel.id === "${TEST_ID}" && S.sel.custom === true && S.trace !== null`);
  check('U10', 'Evaluation of an unsaved edited map is refused; "Save as scenario" stores it and lists it as custom', blocked && saved);

  await ev('document.querySelector(\'input[name="result"][value="INCONCLUSIVE"]\').click()');
  await ev('document.getElementById("evalEvaluator").value = "ui-check"; document.getElementById("evalObs").value = "automated UI check"; document.getElementById("evalTicks").value = "1"');
  await click('saveEvalBtn');
  const evOk = await waitFor('/Saved \\(INCONCLUSIVE\\)/.test(document.getElementById("evalStatus").textContent)');
  const stored = await (await fetch(BASE + '/api/evaluations')).json();
  const rec = stored[TEST_ID];
  const report = fs.readFileSync(path.join(SIM, '..', 'artifacts', 'SystemTest', 'test_results.md'), 'utf8');
  check('U11', 'Human evaluation is recorded with its trace and test_results.md is regenerated',
    evOk && rec && rec.result === 'INCONCLUSIVE' && rec.evaluator === 'ui-check' && rec.trace_file && report.includes(TEST_ID),
    rec ? rec.trace_file : 'no record');

  await click('restartBtn');
  const restarted = await waitFor('S.source === "live run" && S.timer !== null');
  await click('pauseBtn');
  check('U12', 'Restart re-runs the C controller from reset and plays; Pause stops', restarted && (await ev('S.timer === null')));

  await send('Page.navigate', { url: BASE + '/#scenario=ST-05&tick=6' });
  const deep = await waitFor('typeof S !== "undefined" && S.sel && S.sel.id === "ST-05" && S.tick === 6');
  check('U13', 'Deep link #scenario=ST-05&tick=6 opens the scenario at the tick', deep);
  check('U14', 'No uncaught page errors', errors.length === 0, errors.join('; '));
}

function cleanup() {
  try { fs.rmSync(path.join(SIM, 'scenarios', 'custom', TEST_ID + '.json'), { force: true }); } catch (e) { /* */ }
  const tdir = path.join(SIM, 'results', 'traces');
  if (fs.existsSync(tdir)) for (const f of fs.readdirSync(tdir)) if (f.startsWith(TEST_ID + '_')) fs.rmSync(path.join(tdir, f));
  if (evalBackup !== null) fs.writeFileSync(evalFile, evalBackup);
  else fs.rmSync(evalFile, { force: true });
  if (!resultsDirExisted) fs.rmSync(path.join(SIM, 'results'), { recursive: true, force: true });
  if (!customDirExisted) fs.rmSync(path.join(SIM, 'scenarios', 'custom'), { recursive: true, force: true });
  try { execFileSync('python', [path.join(SIM, 'results_report.py')]); } catch (e) { console.error('report regeneration failed'); }
}

try {
  await main();
} catch (e) {
  check('U0', 'UI check ran to completion', false, e.message);
} finally {
  try { ws && ws.close(); } catch (e) { /* */ }
  chrome.kill();
  cleanup();
  const lines = [`Run: ${new Date().toISOString()} · \`node --experimental-websocket simulator/validation/ui_check.mjs\` against ${BASE} (headless Chrome).`, '',
    '| Check | What is verified | Result | Detail |', '|---|---|---|---|',
    ...results.map((r) => `| ${r.id} | ${r.title} | ${r.ok ? 'OK' : 'PROBLEM'} | ${(r.detail || '').replace(/\|/g, '/')} |`), '',
    'Temporary test data (custom scenario, evaluation, trace) was removed after the run.'];
  fs.writeFileSync(path.join(HERE, 'ui_check_result.md'), lines.join('\n') + '\n');
  setTimeout(() => process.exit(results.every((r) => r.ok) ? 0 : 1), 300);
}
