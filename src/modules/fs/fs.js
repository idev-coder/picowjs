process.binding(process.binding.fs); // init native fs

const fs = require('node:fs')
const __path = require("node:path");
const stats = fs.stat()
let __cwd = "/";

exports.cwd = () => {
  return __cwd;
}

exports.chdir = () => {
  const _path = __path.resolve(path);
  const _stat = fs.stat(_path);
  if (_stat && _stat.isDirectory()) {
    __cwd = _path;
  } else {
    throw new SystemError(-2); // ENOENT
  }
}

module.exports = require('node:fs')