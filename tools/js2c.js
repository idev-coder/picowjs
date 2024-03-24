// Generate c source from js files

const fs = require('node:fs');
const path = require('node:path');
const childProcess = require('node:child_process');
const mustache = require("mustache");
const minimist = require("minimist");
const magicStrings = require("./magic_strings");

var modulesPath = path.join(__dirname, "../src/modules");
var targetPath = path.join(__dirname, "../targets");

// Parse modules for generate
var argv = minimist(process.argv.slice(2));

// modules
var modules = [];

generateAll();

function generateAll() {
  console.log("Generating modules for build...");
  console.log();
  identifyModules();
  generateSnapshots();
  generateSources();
  removeWrappers();
  removeSnapshots();
  magicStrings.generateMagicStrings(modules);
}

function identifyModules() {
  var moduleNames = argv.modules.trim().split(" ");
  moduleNames.forEach((moduleName) => {
    console.log("module: " + moduleName + " ------------------------------");
    const modpath = path.join(modulesPath, moduleName);
    const config = JSON.parse(
      fs.readFileSync(path.join(modpath + "/module.json"))
    );
    var module = {
      path: modpath,
      name: moduleName,
      nameUC: moduleName.toUpperCase(),
      js: config.js,
      native: config.native,
      require: config.require,
      size: 0,
    };
    modules.push(module);
  });

  if (argv.target && argv.board) {
    const boardPath = path.join(targetPath, argv.target, "boards", argv.board);
    if (fs.existsSync(path.join(boardPath, "board.js"))) {
      modules.push({
        path: path.join(boardPath),
        name: "board",
        nameUC: "BOARD",
        js: true,
        native: false,
        require: false,
        size: 0,
      });
    }
  }
}

function generateSnapshots() {
  modules.forEach((mod) => {
    if (mod.js) {
      const src = path.join(mod.path, mod.name + ".js");
      const wrapped = path.join(mod.path, mod.name + ".wrapped");
      const snapshot = path.join(mod.path, mod.name + ".snapshot");
      mod.wrapped = wrapped;
      mod.snapshot = snapshot;
      createWrapper(src, wrapped);
      createSnapshot(wrapped, snapshot);
    }
  });
}

function createWrapper(src, dest) {
  const wrapper_header = "(function(exports, require, module) {\n";
  const wrapper_footer = "\n});\n";
  const data = fs.readFileSync(src, "utf8");
  fs.writeFileSync(dest, wrapper_header + data + wrapper_footer, "utf8");
}

function createSnapshot(src, dest) {
  childProcess.spawnSync(
    "lib/jerryscript/build/bin/jerry-snapshot",
    ["generate", src, "-o", dest],
    { stdio: "inherit" }
  );
}

function removeWrappers() {
  modules.forEach((mod) => {
    if (mod.wrapped) {
      fs.unlinkSync(mod.wrapped);
    }
  });
}

function removeSnapshots() {
  modules.forEach((mod) => {
    if (mod.snapshot) {
      fs.unlinkSync(mod.snapshot);
    }
  });
}

function generateSources() {
  const template_h = fs.readFileSync(
    __dirname + "/picowjs_modules.h.mustache",
    "utf8"
  );
  const template_c = fs.readFileSync(
    __dirname + "/picowjs_modules.c.mustache",
    "utf8"
  );
  // Convert snapshot to an array of byte.
  modules.forEach((mod) => {
    if (mod.snapshot) {
      var buffer = fs.readFileSync(mod.snapshot);
      var hex = buffer.toString("hex");
      var segments = hex.match(/.{1,20}/g);
      mod.size = buffer.length;
      mod.segments = [];
      segments.forEach((segment, index) => {
        var bytes = segment.match(/.{1,2}/g).map((item) => ({ value: item }));
        if (index == segments.length - 1) {
          bytes[bytes.length - 1].last = true;
        }
        mod.segments.push({ bytes: bytes });
      });
    }
  });
  modules[modules.length - 1].lastModule = true;
  var builtinModules = [];
  modules.forEach((mod) => {
    if (mod.require) {
      builtinModules.push(mod);
    }
  });
  var view = { modules: modules, builtinModules: builtinModules };
  var rendered_h = mustache.render(template_h, view);
  var rendered_c = mustache.render(template_c, view);
  var genPath = path.join(__dirname, "../src/gen");
  fs.mkdirSync(genPath);
  fs.writeFileSync(path.join(genPath, "picowjs_modules.h"), rendered_h, "utf8");
  fs.writeFileSync(path.join(genPath, "picowjs_modules.c"), rendered_c, "utf8");
}
