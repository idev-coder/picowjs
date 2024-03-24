// Generate c source from js files

const fs = require('node:fs');
const os = require('node:os'); 
const path = require('node:path'); 
const minimist = require("minimist");
const childProcess = require('node:child_process');

// Parse options
var argv = minimist(process.argv.slice(2));


argv.target = argv.target ? argv.target : "rp2"
argv.board = argv.board ? argv.board : "pico-w"

const buildPath = path.join(__dirname, "build");
const srcGenPath = path.join(__dirname, "src/gen");

if (argv.clean) {
  clean();
} else {
  build();
}

function clean() {
  fs.rmSync(buildPath);
  fs.rmSync(srcGenPath);
}

function build() {
  // ensure /build
  fs.mkdirSync(buildPath);

  // execute cmake and make
  process.chdir(buildPath);
  const params = [".."];
  if (argv.target) params.push(`-DTARGET=${argv.target}`);
  if (argv.board) params.push(`-DBOARD=${argv.board}`);
  if (argv.modules) params.push(`-DMODULES=${argv.modules}`);

  // build everything
  const cores = os.cpus().length;
  cmd("cmake", params);
  cmd("make", [`-j${cores}`]);
  process.chdir(__dirname);
}

function cmd(cmd, args) {
  childProcess.spawnSync(cmd, args, { stdio: "inherit" });
}
