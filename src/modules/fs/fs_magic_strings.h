/* Copyright (c) 2024 Pico-W-JS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __FS_MAGIC_STRINGS_H
#define __FS_MAGIC_STRINGS_H

#define MSTR_FS "fs"
#define MSTR_FS_ACCESS "access"
#define MSTR_FS_APPEND_FILE "appendFile"
#define MSTR_FS_CHMOD "chmod"
#define MSTR_FS_CHOWN "chown"
#define MSTR_FS_CLOSE "close"
#define MSTR_FS_COPY_FILE "copyFile"
#define MSTR_FS_CP "cp"
#define MSTR_FS_CREATE_READ_STREAM "createReadStream"
#define MSTR_FS_CREATE_WRITE_STREAM "createWriteStream"
#define MSTR_FS_EXISTS "exists"
#define MSTR_FS_FCHMOD "fchmod"
#define MSTR_FS_FCHOWN "fchown"
#define MSTR_FS_FDATASYNC "fdatasync"
#define MSTR_FS_FSTAT "fstat"
#define MSTR_FS_FSYNC "fsync"
#define MSTR_FS_FTRUNCATE "ftruncate"
#define MSTR_FS_FUTIMES "futimes"
#define MSTR_FS_LCHMOD "lchmod"
#define MSTR_FS_LCHOWN "lchown"
#define MSTR_FS_LUTIMES "lutimes"
#define MSTR_FS_LINK "link"
#define MSTR_FS_LSTAT "lstat"
#define MSTR_FS_MKDIR "mkdir"
#define MSTR_FS_MKDTEMP "mkdtemp"
#define MSTR_FS_OPEN "open"
#define MSTR_FS_OPEN_AS_BLOB "openAsBlob"
#define MSTR_FS_OPENDIR "opendir"
#define MSTR_FS_READ "read"
#define MSTR_FS_READDIR "readdir"
#define MSTR_FS_READ_FILE "readFile"
#define MSTR_FS_READLINK "readlink"
#define MSTR_FS_READV "readv"
#define MSTR_FS_READPATH "readpath"
#define MSTR_FS_RENAME "rename"
#define MSTR_FS_RMDIR "rmdir"
#define MSTR_FS_RM "rm"
#define MSTR_FS_STAT "stat"
#define MSTR_FS_STATFS "statfs"
#define MSTR_FS_SYMLINK "symlink"
#define MSTR_FS_TRUNCATE "truncate"
#define MSTR_FS_UNLINK "unlink"
#define MSTR_FS_UNWATCH_FILE "unwatchFile"
#define MSTR_FS_UTIMES "utimes"
#define MSTR_FS_WATCH "watch"
#define MSTR_FS_WATCH_FILE "watchFile"
#define MSTR_FS_WRITE "write"
#define MSTR_FS_WRITE_FILE "writeFile"
#define MSTR_FS_WRITEV "writev"

#define MSTR_FS_ACCESS_SYNC "accessSync"
#define MSTR_FS_APPEND_FILE_SYNC "appendFileSync"
#define MSTR_FS_CHMOD_SYNC "chmodSync"
#define MSTR_FS_CHOWN_SYNC "chownSync"
#define MSTR_FS_CLOSE_SYNC "closeSync"
#define MSTR_FS_COPY_FILE_SYNC "copyFileSync"
#define MSTR_FS_CP_SYNC "cpSync"
#define MSTR_FS_EXISTS_SYNC "existsSync"
#define MSTR_FS_FCHMOD_SYNC "fchmodSync"
#define MSTR_FS_FCHOWN_SYNC "fchownSync"
#define MSTR_FS_FDATASYNC_SYNC "fdatasyncSync"
#define MSTR_FS_FSTAT_SYNC "fstatSync"
#define MSTR_FS_FSYNC_SYNC "fsyncSync"
#define MSTR_FS_FTRUNCATE_SYNC "ftruncateSync"
#define MSTR_FS_FUTIMES_SYNC "futimesSync"
#define MSTR_FS_LCHMOD_SYNC "lchmodSync"
#define MSTR_FS_LCHOWN_SYNC "lchownSync"
#define MSTR_FS_LUTIMES_SYNC "lutimesSync"
#define MSTR_FS_LINK_SYNC "linkSync"
#define MSTR_FS_LSTAT_SYNC "lstatSync"
#define MSTR_FS_MKDIR_SYNC "mkdirSync"
#define MSTR_FS_MKDTEMP_SYNC "mkdtempSync"
#define MSTR_FS_OPEN_SYNC "openSync"
#define MSTR_FS_OPENDIR_SYNC "opendirSync"
#define MSTR_FS_READ_SYNC "readSync"
#define MSTR_FS_READDIR_SYNC "readdirSync"
#define MSTR_FS_READ_FILE_SYNC "readFileSync"
#define MSTR_FS_READLINK_SYNC "readlinkSync"
#define MSTR_FS_READV_SYNC "readvSync"
#define MSTR_FS_READPATH_SYNC "readpathSync"
#define MSTR_FS_RENAME_SYNC "renameSync"
#define MSTR_FS_RMDIR_SYNC "rmdirSync"
#define MSTR_FS_RM_SYNC "rmSync"
#define MSTR_FS_STAT_SYNC "statSync"
#define MSTR_FS_STATFS_SYNC "statfsSync"
#define MSTR_FS_SYMLINK_SYNC "symlinkSync"
#define MSTR_FS_TRUNCATE_SYNC "truncateSync"
#define MSTR_FS_UNLINK_SYNC "unlinkSync"
#define MSTR_FS_UTIMES_SYNC "utimesSync"
#define MSTR_FS_WATCH_FILE_SYNC "watchFileSync"
#define MSTR_FS_WRITE_SYNC "writeSync"
#define MSTR_FS_WRITE_FILE_SYNC "writeFileSync"
#define MSTR_FS_WRITEV_SYNC "writevSync"

#define MSTR_FS_CWD ="cwd"
#define MSTR_FS_CHDIR "chdir"

#define MSTR_FS_STATS_IS_DIRECTORY "isDirectory"
#define MSTR_FS_STATS_SIZE "size"


#endif /* __FS_MAGIC_STRINGS_H */
