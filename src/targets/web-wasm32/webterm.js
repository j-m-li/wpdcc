var term = 0;

function WebTerm(div, pw, ph, out) {
  if (term !== 0) {
    return term;
  }
  term = this;
  this.out = out;
  this.div = div;
  this.class_ = "";
  this.escape_state = 0;
  this.escape_val = 0;
  this.escape_args = new Array();
  this.escape_save = new Array();
  this.escape_gfx = document.createElement('span');
  this.safe = getComputedStyle(document.documentElement).getPropertyValue(
    "--sat");
  s = getComputedStyle(document.documentElement).getPropertyValue("--sar");
  if (s > this.safe) {
    this.safe = s;
  }
  s = getComputedStyle(document.documentElement).getPropertyValue("--sab");
  if (s > this.safe) {
    this.safe = s;
  }
  s = getComputedStyle(document.documentElement).getPropertyValue("--sal");
  if (s > this.safe) {
    this.safe = s;
  }
  if (isNaN(this.safe)) {
    this.safe = 5;
  }
  var w = 80;
  var h = 25;
  for (var i = 0; i < h; i++) {
    var row = document.createElement('div');
    for (var j = 0; j < w; j++) {
      var cell = document.createElement('span');
      var txt = document.createTextNode("\u{a0}");
      cell.classList.add("hidden");
      cell.appendChild(txt);
      row.appendChild(cell);
    }
    div.appendChild(row);
  }
  this.x = 0;
  this.y = 0;
  this.w = w;
  this.h = h;
  this.reset();
  var cursor = this.div.children[0].children[0];
  cursor.classList.add("cursor");
  this.resize(pw, ph);
  var viewport = document.querySelector('meta[name="viewport"]');
  if (viewport) {
    //viewport.content = 'width=device-width, initial-scale=1';
    //viewport.content = 'width=device-width';
  }
  return this;
}
WebTerm.prototype.reset = function() {
  var cursor = this.div.children[this.y].children[this.x];
  cursor = this.escape_gfx;
  cursor.classList.add("cursor");
  cursor.style.setProperty("font-style", "normal"); // "italic"
  cursor.style.setProperty("font-weight", "normal"); // "bold"
  cursor.style.setProperty("text-decoration-line", "none"); // "underline"
  cursor.style.setProperty("color", "#FFFFFF");
  cursor.style.setProperty("background-color", "#000000");
  this.style = cursor.style;
  this.class_ = cursor.getAttribute("class");
}
// Select Graphics Rendition
// https://en.wikipedia.org/wiki/ANSI_escape_code
WebTerm.prototype.sgr = function() {
  var f = 0;
  if (this.escape_args.length > 1) {
    f = this.escape_args[0];
  }
  if (f == 0) {
    this.reset();
  } else if (f == 1) {
    this.escape_gfx.style.setProperty("font-weight", "bold");
  } else if (f == 2) {
    // do nothing
  } else if (f == 3) {
    this.escape_gfx.style.setProperty("font-style", "italic");
  } else if (f == 4) {
    this.escape_gfx.style.setProperty("text-decoration-line", "underline");
  } else if (f >= 30 && f <= 37) {
    this.escape_gfx.style.setProperty("color", this.get_ansi_color(f - 30));
  } else if (f >= 40 && f <= 47) {
    this.escape_gfx.style.setProperty("background-color", this
      .get_ansi_color(f - 40));
  } else if (f >= 90 && f <= 97) {
    // FIXME brighter
    this.escape_gfx.style.setProperty("color", this.get_ansi_color(f - 90));
  } else if (f >= 100 && f <= 107) {
    // FIXME brighter
    this.escape_gfx.style.setProperty("background-color", this
      .get_ansi_color(f - 100));
  }
}
WebTerm.prototype.ansi_private = function(c) {
  var cursor = this.div.children[this.y].children[this.x];
  if (c == 104) { //'h' show caret
    if (this.escape_args.length > 1) {
      var cmd = this.escape_args[1];
      if (cmd == 25) {
        cursor.classList.add("cursor");
      }
    }
  } else if (c == 108) { // 'l' hide caret
    if (escape_args.size > 1) {
      var cmd = escape_args[1]
      if (cmd == 25) {
        cursor.classList.remove("cursor");
      }
    }
  }
}
/* parse escape sequences */
WebTerm.prototype.ansi_term = function(c) {
  // https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
  // https://sourceforge.net/p/pdos/gitcode/ci/master/tree/src/pdos.c#l5493
  var n = 1;
  if (this.escape_args.length > 0) {
    if (this.escape_args[0] == -63) { // '?'
      this.ansi_private(c)
      return
    }
    n = this.escape_args[0];
  }
  if (c == 65) { // 'A'
    this.y -= n;
  } else if (c == 66) { // 'B'
    this.y += n;
  } else if (c == 67) { // 'C'
    this.x += n;
  } else if (c == 68) { // 'D'
    this.x -= n;
  } else if (c == 69) { // 'E'
    this.y += n;
    this.x = 0;
  } else if (c == 70) { // 'F'
    this.y -= n;
    this.x = 0;
  } else if (c == 71 || c == 102) { // 'G' || 'f'
    if (this.escape_args.length > 0) {
      this.x = this.escape_args[0] - 1;
    }
  } else if (c == 72) { // 'H' set cursor position
    if (this.escape_args.length > 0) {
      this.y = this.escape_args[0] - 1;
      if (this.escape_args.length > 1) {
        this.x = this.escape_args[1] - 1;
      } else {
        this.x = 0;
      }
    }
  } else if (c == 74) { // 'J' clear screen
    if (this.escape_args.length == 0) {
      n = 0;
    }
    if (n == 0) {
      this.earse(this.x, this.y, this.w - 1, this.h - 1);
    } else if (n == 1) {
      this.earse(0, 0, this.x, this.y);
    } else if (n == 2) {
      this.earse(0, 0, this.w - 1, this.y - 1);
    }
  } else if (c == 75) { // 'K' clear line
    if (this.escape_args.length == 0) {
      n = 0;
    }
    if (n == 0) {
      this.earse(this.x, this.y, this.w - 1, this.y);
    } else if (n == 1) {
      this.earse(0, this.y, this.x, this.y);
    } else if (n == 2) {
      earse(0, this.y, this.w - 1, this.y);
    }
  } else if (c == 83) { // 'S'
    this.scroll(n);
  } else if (c == 84) { // 'T'
    this.scroll(-n);
  } else if (c == 99) { // 'c'
    if (n == 0) { // Device attributes: VT101 with no options
      this.output("\u001b[?1;0c");
    }
  } else if (c == 115) { // 's'
    this.escape_save.push(this.y)
    this.escape_save.push(this.x);
  } else if (c == 117) { // 'u'
    if (this.escape_save.length > 1) {
      this.x = this.escape_save.pop();
      this.y = this.escape_save.pop();
    }
  } else if (c == 109) { // 'm'
    this.sgr();
  } else if (c == 110) { // 'n'
    if (n == 6) { // report cursor position
      this.output("\u001b[" + (this.y + 1) + ";" + (this.x + 1) + "R");
    }
  }
}
WebTerm.prototype.resize = function(pw, ph) {
  if (pw > 320) {
    pw -= 2 * this.safe;
    ph -= 2 * this.safe;
  }
  var w = 80;
  var h = 25;
  if (pw < 960) {
    w = 40;
  }
  if (ph < 600) {
    h = 15;
  }
  this.w = w;
  this.h = h;
  for (var i = 0; i < 25; i++) {
    var row = this.div.children[i];
    for (var j = 0; j < 80; j++) {
      var cell = row.children[j];
      if (i < h && j < w) {
        cell.classList.remove("hidden");
      } else {
        cell.classList.add("hidden");
      }
    }
  }
  var scale = ph / (h * 16);
  var lh = 16 * scale;
  lh = Math.floor(lh);
  var lw = 8 * scale;
  if (lw * w > pw) {
    scale = pw / (w * lw);
    lh = lh * scale;
    lh = Math.floor(lh);
  }
  if (lh < 16) {
    lh = 16;
  } else if (lh > 24) {
    //lh = 24;
  }
  var fs = Math.floor(lh * 20 / 24);
  fs = lh / 1.3;
  this.div.style.fontSize = fs + "px";
  this.div.style.lineHeight = lh + "px";
  if (pw > 320) {
    this.div.style.padding = this.safe + "px";
  }
  var dw = this.div.getBoundingClientRect().width;
  var dh = this.div.getBoundingClientRect().height;
  if (pw > dw && pw > 320) {
    this.div.style.paddingLeft = this.safe + (pw - dw) / 2 + "px";
  }
}
WebTerm.prototype.set_style = function(cell) {
  cell.setAttribute('class', this.escape_gfx.getAttribute('class'));
  var style = this.escape_gfx.style;
  cell.classList.remove("cursor");
  var p = new Array("color", "background-color", "font-style", "font-weight",
    "text-decoration-line");
  p.forEach(function(item) {
    var s = style.getPropertyValue(item);
    cell.style.setProperty(item, s);
  });
}
WebTerm.prototype.get_ansi_color = function(index) {
  switch (index) {
    case 0:
      return "Black";
    case 1:
      return "Red";
    case 2:
      return "Green";
    case 3:
      return "Yellow";
    case 4:
      return "Blue";
    case 5:
      return "Magenta";
    case 6:
      return "Cyan";
    case 7:
      return "White";
    default:
      return "Gray";
  }
}
WebTerm.prototype.earse = function(scol, srow, ecol, erow) {
  var r = srow;
  while (r <= erow) {
    var row = this.div.children[r];
    var c = 0;
    var ec = this.w;
    if (r == srow) {
      c = scol;
    }
    if (r == erow) {
      ec = ecol;
    }
    while (c <= ec) {
      var cell = row.children[c];
      cell.innerText = "\u{a0}";
      if (c < this.w && this.h == 25) {
        this.set_style(cell);
      } else {
        cell.setAttribute('class', "hidden");
      }
      c++;
    }
    r++;
  }
}
WebTerm.prototype.scroll = function(amount) {
  while (amount < 0) {
    var row = this.div.lastChild;
    this.div.removeChild(row);
    for (var j = 0; j < 80; j++) {
      var cell = row.children[j];
      cell.innerText = "\u{a0}";
      if (j < this.w && this.h == 25) {
        this.set_style(cell);
      } else {
        cell.setAttribute('class', "hidden");
      }
    }
    this.div.insertBefore(this.div.firstChild, row);
    row = this.div.children[this.h - 1];
    for (var j = 0; j < 80; j++) {
      var cell = row.children[j];
      cell.innerText = "\u{a0}";
      if (j < this.w) {
        this.set_style(cell);
      }
    }
  }
  while (amount > 0) {
    amount--;
    var row = this.div.firstChild;
    this.div.removeChild(row);
    for (var j = 0; j < 80; j++) {
      var cell = row.children[j];
      cell.innerText = "\u{a0}";
      if (j < this.w && this.h == 25) {
        this.set_style(cell);
      } else {
        cell.setAttribute('class', "hidden");
      }
    }
    this.div.appendChild(row);
    row = this.div.children[this.h - 1];
    for (var j = 0; j < 80; j++) {
      var cell = row.children[j];
      cell.innerText = "\u{a0}";
      if (j < this.w) {
        this.set_style(cell);
      }
    }
  }
}
// https://sourceforge.net/p/pdos/gitcode/ci/master/tree/src/pdos.c#l1764
WebTerm.prototype.keydown = function(code, shift, ctrl, alt, meta) {
  var s = "";
  switch (code) {
    //	case 'Enter':
    //	case 'NumpadEnter': s = "\n"; break;
    case 'Tab':
      s = "\t";
      break;
    case 'Escape':
      s = "\u001b\u001b";
      break;
      //	case 'Backspace': s = "\b"; break;
    case 'Delete':
      s = "\u001b[3~";
      break;
    case 'ArrowUp':
      s = "\u001b[A";
      break;
    case 'ArrowDown':
      s = "\u001b[B";
      break;
    case 'ArrowRight':
      if (ctrl) {
        s = "\u001b[1;5C";
        break;
      } else {
        s = "\u001b[C";
        break;
      }
    case 'ArrowLeft':
      if (ctrl) {
        s = "\u001b[1;5D";
        break;
      } else {
        s = "\u001b[D";
        break;
      }
    case 'Insert':
      s = "\u001b[2~";
      break;
    case 'Home':
      s = "\u001b[1~";
      break;
    case 'End':
      s = "\u001b[4~";
      break;
    case 'PageDown':
      if (ctrl) {
        s = "\u001b[6;5~";
        break;
      } else {
        s = "\u001b[6~";
        break;
      }
    case 'PageUp':
      if (ctrl) {
        s = "\u001b[5;5~";
        break;
      } else {
        s = "\u001b[5~";
        break;
      }
  }
  if (alt) {
    if (code.startsWith("Key")) {
      var k = code.codePointAt(3);
      if (k >= 65 && k <= 90) { // 'A' && 'Z'
        // k + 'a'
        s = "\u001b" + String.fromCodePoint(k + 97);
      }
    }
  }
  if (ctrl && s.length == 0) {
    var k = code.codePointAt(3);
    if (k >= 65 && k <= 90) { // 'A' && 'Z'
      s = String.fromCodePoint(k + 1);
    }
  }
  return s;
}
// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
WebTerm.prototype.add_cp = function(c) {
  var i = 1;
  if (this.escape_state == 3) {
    this.escape_state = 0;
  }
  if (this.escape_state == 1) {
    // https://notes.burke.libbey.me/ansi-escape-codes/
    if (c == 91) { // '['
      this.escape_state = 2;
      this.escape_val = -1;
      this.escape_args.length = 0;
    } else {
      this.escape_state = 0;
      i--; // rescan the current character
    }
  } else if (this.escape_state == 2) {
    if (c >= 48 && c <= 57) { //'0' && '9'
      if (this.escape_val == -1) {
        this.escape_val = 0;
      }
      this.escape_val *= 10;
      this.escape_val += c - 48; // c - '0'
    } else if (c == 59) { // ';'
      this.escape_args.push(this.escape_val);
      this.escape_val = -1;
    } else if (c == 61) { // '='
      this.escape_args.push(-61);
      this.escape_val = -1;
    } else if (c == 63) { // '?'
      this.escape_args.push(-63);
      this.escape_val = -1;
    } else if (c >= 0x40 && c <= 0x7E) { // '@' && '~'
      if (this.escape_val >= 0) {
        this.escape_args.push(this.escape_val);
      }
      this.ansi_term(c);
      this.escape_state = 3;
    }
  } else if (c == 10) { // \n
    this.y++;
    this.x = 0;
  } else if (c == 13) { // \r
  } else if (c == 8) { // \b
    if (this.x > 0) {
      this.x--;
      this.set_text("\u{a0}", this.x, this.y);
    }
  } else if (c == 9) { // \t
    var nx = this.x + 8 - (this.x % 8);
    while (this.x < nx && this.x < this.w) {
      this.set_text("\u{a0}", this.x, this.y);
      this.x++;
    }
  } else if (c == 0x1B) { // ESC
    this.escape_state = 1;
  } else {
    this.set_text(String.fromCodePoint(c), this.x, this.y);
    this.x++;
  }
  if (this.x < 0) {
    this.x = 0;
  }
  if (this.y < 0) {
    this.y = 0;
  }
  if (this.x >= this.w) {
    if (this.escape_state == 0) {
      this.y++;
      this.x = 0;
    } else {
      this.x = this.w - 1;
    }
  }
  if (this.y >= this.h) {
    if (this.escape_state == 0) {
      this.scroll(this.y - this.h + 1);
    }
    this.y = this.h - 1;
  }
  return i;
}
WebTerm.prototype.output = function(txt) {
  this.out.send(txt);
}
WebTerm.prototype.input = function(txt) {
  this.add_txt(txt);
}
WebTerm.prototype.add_txt = function(txt) {
  var cursor = this.div.children[this.y].children[this.x];
  this.style = cursor.style;
  this.class_ = cursor.getAttribute("class");
  cursor.classList.remove("cursor");
  for (var i = 0; i < txt.length;) {
    var c = txt.codePointAt(i);
    if (c >= 0xDC00 && c <= 0xDFFF) {
      // trailing surrogate
      i++;
    } else {
      i += this.add_cp(c);
    }
  }
  cursor = this.div.children[this.y].children[this.x];
  this.set_style(cursor);
  cursor.classList.add("cursor");
}
WebTerm.prototype.set_text = function(txt, x, y) {
  var cell = this.div.children[y].children[x];
  var cursor = this.div.children[this.y].children[this.x];
  var style = cursor.style;
  var class_ = cursor.getAttribute("class");
  if (txt.length < 1 || txt == " ") {
    txt = "\u{a0}";
  }
  cell.innerText = txt;
  this.set_style(cell);
}
var Out = function() {
  this.cmd = "";
  this.tmp = "";
  this.tmp1 = "";
  this.index = 0;
  this.session = 0;
  this.term = 0;
  this.state = 'uninit';
  return this;
}
Out.prototype.set_session = function(s) {
  this.session = s;
}
Out.prototype.set_term = function(t) {
  this.term = t;
}
Out.prototype.get_cmd = function() {
  var t = this.cmd;
  this.cmd = "";
  this.index = 0;
  return t;
}
Out.prototype.add = function(txt) {
  var r = "";
  while (txt.codePointAt(0) == 8) {
    if (this.index > 0) {
      this.cmd = this.cmd.substr(0, this.index - 1) + this.cmd.substr(this
        .index, this.cmd.length - 1 - this.index);
    } else {
      this.cmd = this.cmd.substr(0, this.cmd.length - 1);
    }
    txt = txt.substr(1);
    if (this.index > 0) {
      this.index--;
      r += "\b";
    }
  }
  if (txt[0] == "\u001b") {
    switch (txt) { // FIXME
      case "\u001b[D":
        if (this.index > 0) {
          this.index--;
        }
        break;
      case "\u001b[C":
        if (this.index < this.cmd.length) {
          this.index++;
        }
        break;
    }
  } else {
    this.index += txt.length;
    this.cmd += txt;
    if (txt.length > 0) {
      r += "*";
    }
  }
  return r;
}
Out.prototype.send = function(txt) {
  keyboard({
    data: txt
  });
  return;
  switch (this.state) {
    case 'uninit':
      this.get_cmd();
      break;
    case 'user':
      this.term.add_txt(txt);
      if (txt == "\n") {
        this.session.user = this.get_cmd();
        this.cb(200, "\n", 0);
      } else {
        this.add(txt);
      }
      break;
    case 'password':
      if (txt == "\n") {
        this.session.password = this.get_cmd();
        this.term.add_txt("\n");
        this.cb(200, "\n", 0);
      } else {
        this.term.add_txt(this.add(txt));
      }
      break;
    case 'adduser1':
      if (txt == "\n") {
        this.tmp1 = this.get_cmd();
        this.tmp = "";
        this.term.add_txt("\nNew user password: ");
        this.state = 'adduser2';
      } else {
        this.term.add_txt(txt);
        this.add(txt);
      }
      break;
    case 'passwd1':
    case 'adduser2':
      if (txt == "\n") {
        this.tmp = this.get_cmd();
        this.term.add_txt("\nRetype new password: ");
        if (this.state == "passwd1") {
          this.state = 'passwd2';
        } else {
          this.state = 'adduser3';
        }
      } else {
        this.term.add_txt(this.add(txt));
      }
      break;
    case 'passwd2':
    case 'adduser3':
      if (txt == "\n") {
        var p = this.get_cmd();
        if (p !== this.tmp) {
          this.term.add_txt("\nError passwords don't match.\n");
          this.state = 'idle';
        } else {
          this.term.add_txt("\n");
          if (this.state == "passwd2") {
            std.passwd(this.session, p, this.cb, this);
          } else {
            std.adduser(this.session, this.tmp1, p, this.cb, this);
          }
        }
      } else {
        this.term.add_txt(this.add(txt));
      }
      break;
    default:
      if (txt == "\n") {
        this.term.add_txt(txt);
        var cmd = this.get_cmd();
        var args = cmd.split(' ');
        var a = ".";
        var ax = "";
        if (args.length > 1) {
          a = args[1];
        }
        for (var i = 2; i < args.length; i++) {
          if (i > 2) {
            ax += " ";
          }
          ax += args[i];
        }
        ax += "\n";
        switch (args[0]) {
          case "exit":
          case "quit":
          case "logout":
          case "logoff":
          case "reboot":
          case "shutdown":
            std.logout(this.session, this.cb, this);
            this.state = 'logout';
            break;
          case "passwd":
          case "changepassword":
            this.state = 'passwd1';
            this.tmp = "";
            this.term.add_txt("New password: ");
            break;
          case "adduser":
          case "useradd":
            this.tmp1 = "";
            this.tmp = "";
            this.state = 'adduser1';
            this.term.add_txt("New user name: ");
            break;
          case 'help':
          case '?':
            this.term.add_txt("Available commands:\n");
            this.term.add_txt("\tlist adduser passwd\n");
            this.term.add_txt("\tmkdir rmdir filesize\n");
            this.term.add_txt("\twrite show remove\n");
            this.term.add_txt("\texit\n\n");
            break;
          case "ls":
          case "dir":
          case "list":
            this.state = 'cmd';
            std.scandir(this.session, a, this.cb, this);
            break;
          case "mkdir":
          case "md":
            this.state = 'cmd';
            std.mkdir(this.session, a, this.cb, this);
            break;
          case "rmdir":
            this.state = 'cmd';
            std.rmdir(this.session, a, this.cb, this);
            break;
          case "rm":
          case "del":
          case "remove":
          case "unlink":
            this.state = 'cmd';
            std.unlink(this.session, a, this.cb, this);
            break;
          case "filesize":
          case "du":
            this.state = 'cmd';
            std.filesize(this.session, a, this.cb, this);
            break;
          case "cat":
          case "type":
          case "show":
          case "more":
            this.state = 'cmd';
            std.read(this.session, a, 0, 0, -1, this.cb, this);
            break;
          case "write":
          case "touch":
          case "append":
            this.state = 'cmd';
            std.write(this.session, a, ax, 0, -1, -1, this.cb, this);
            break;
          default:
            std.echo(this.session, cmd, this.cb, this);
            console.log("'" + cmd + "'");
        }
      } else {
        this.term.add_txt(txt);
        this.add(txt);
      }
  }
}
Out.prototype.cb = function(status, response, type) {
  var txt = "";
  switch (this.state) {
    case 'uninit':
      txt = "Please login as 'guest' and\ncreate an account.\n\nLogin: ";
      this.state = 'user';
      this.session.user = "";
      break;
    case 'user':
      this.state = 'password';
      this.session.password = "";
      txt = "Password: ";
      break;
    case 'password':
      this.state = 'login';
      txt = "";
      std.login(this.session, this.cb, this);
      break;
    case 'login':
      if (status == 200) {
        txt = "GREETINGS PROFESSOR FALKEN\n";
        this.state = 'idle';
      } else {
        txt = response + "\nLogin: ";
        this.state = 'user';
        this.session.user = "";
      }
      break;
    case 'passwd2':
      if (status == 200) {
        txt = "Password successfully changed.\n";
        this.state = 'idle';
      } else {
        txt = response + "\nPassword not changed\n";
        this.state = 'idle';
      }
      break;
    case 'adduser3':
      if (status == 200) {
        txt = "new user successfully created.\n";
        this.state = 'idle';
      } else {
        txt = response + "\nUser not created\n";
        this.state = 'idle';
      }
      break;
    case 'logout':
      txt = response + "\nLogin: ";
      this.state = 'user';
      this.session.user = "";
      break;
    case 'cmd':
      txt = response + "\n";
      this.state = 'idle';
      break;
    case 'idle':
    default:
      txt = response;
  }
  if (this.state != 'user' && status == 403) {
    txt = txt + "\nLogin: ";
    this.state = 'user';
    this.session.user = "";
  }
  if (txt.length > 0) {
    this.term.add_txt(txt);
  }
}

function std() {}
std.init = function(url, cb, this_cb) {
  var t = {};
  t.url = url;
  t.user = "";
  t.password = "";
  t.id = 0;
  std.request(t, "init", "", "", 0, -1, -1, cb, this_cb);
  return t;
}
std.echo = function(sess, txt, cb, this_cb) {
  std.request(sess, "echo", "", txt, 0, -1, -1, cb, this_cb);
}
std.write = function(sess, filename, data, type, seek, size, cb, this_cb) {
  std.request(sess, "write", filename, data, type, seek, size, cb, this_cb);
}
std.read = function(sess, filename, type, seek, size, cb, this_cb) {
  std.request(sess, "read", filename, "", type, seek, size, cb, this_cb);
}
std.unlink = function(sess, filename, cb, this_cb) {
  std.request(sess, "unlink", filename, "", 0, -1, -1, cb, this_cb);
}
std.filesize = function(sess, filename, cb, this_cb) {
  std.request(sess, "filesize", filename, "", 0, -1, -1, cb, this_cb);
}
std.mkdir = function(sess, filename, cb, this_cb) {
  std.request(sess, "mkdir", filename, "", 0, -1, -1, cb, this_cb);
}
std.rmdir = function(sess, filename, cb, this_cb) {
  std.request(sess, "rmdir", filename, "", 0, -1, -1, cb, this_cb);
}
std.scandir = function(sess, dir, cb, this_cb) {
  console.log(dir);
  std.request(sess, "scandir", dir, "", 0, -1, -1, cb, this_cb);
}
std.login = function(sess, cb, this_cb) {
  sess.password = std.sha256("cod5.com" + sess.user + sess.password);
  std.request(sess, "login", "", 0, 0, -1, -1, cb, this_cb);
}
std.logout = function(sess, cb, this_cb) {
  std.request(sess, "logout", "", 0, 0, -1, -1, cb, this_cb);
}
std.adduser = function(sess, name, passwd, cb, this_cb) {
  var p = std.sha256("cod5.com" + name + passwd);
  std.request(sess, "adduser", name, p, 0, -1, -1, cb, this_cb);
}
std.passwd = function(sess, passwd, cb, this_cb) {
  var p = std.sha256("cod5.com" + sess.user + passwd);
  std.request(sess, "passwd", "", p, 0, -1, -1, cb, this_cb);
}
std.request = function(ses, func, name, data, type, seek, size, cb, this_cb) {
  var xhr = new XMLHttpRequest();
  var fd = new FormData();
  fd.set("PHPSESSID", ses.id);
  fd.set("seek", seek);
  fd.set("size", size);
  fd.set("user", ses.user);
  if (func == "login") {
    fd.set("password", ses.password);
  }
  if (func != 'write') {
    fd.set("data", data);
  } else {
    if (type == 0) {
      fd.set("file", new Blob([data.toString()], {
        type: "text/plain; charset=utf-8"
      }));
    } else {
      fd.set("file", new Blob(data, {
        type: "text/plain; charset=x-user-defined"
      }));
    }
  }
  xhr.open('POST', ses.url + "/" + func + "/?" + encodeURIComponent(name));
  xhr.onreadystatechange = function() {
    if (xhr.readyState === 4) {
      ses.id = xhr.getResponseHeader("Session-Id");
      //		alert(ses.id);
      cb.bind(this_cb)(xhr.status, xhr.response, type);
    }
  }
  if (type == 0) {
    xhr.overrideMimeType("text/plain; charset=utf-8");
    xhr.responseType = "text";
  } else {
    xhr.overrideMimeType("text/plain; charset=x-user-defined");
    xhr.responseType = "arraybuffer";
  }
  xhr.send(fd);
}
std.byte2string = function(j) {
  if (j == 0) {
    return '\x00';
  }
  return String.fromCharCode(j & 0xFF);
}
std.string2string8 = function(utf16) {
  var r = "";
  var i;
  var j;
  for (i = 0; i < utf16['length']; i++) {
    j = utf16.codePointAt(i);
    if (j < 0x80) {
      r += std.byte2string(j);
    } else if (j >= 0xDC00 && j <= 0xDFFF) {
      // ignore trailing surrogate
    } else if (j <= 0x07FF) {
      r += std.byte2string(0xC0 | ((j >> 6) & 0x1F));
      r += std.byte2string(0x80 | (j & 0x3F));
    } else if (j <= 0xFFFF) {
      r += std.byte2string(0xE0 | ((j >> 12) & 0x0F));
      r += std.byte2string(0x80 | ((j >> 6) & 0x3F));
      r += std.byte2string(0x80 | (j & 0x3F));
    } else if (j <= 0x10FFFF) {
      r += std.byte2string(0xF0 | ((j >> 18) & 0x07));
      r += std.byte2string(0x80 | ((j >> 12) & 0x3F));
      r += std.byte2string(0x80 | ((j >> 6) & 0x3F));
      r += std.byte2string(0x80 | (j & 0x3F));
    } else {
      console.log("error in string to utf8");
    }
  }
  return r;
}
// https://github.com/geraintluff/sha256
// Input must be an ASCII string - if character codes outside the range 0-255 are received, undefined is returned.
// This library is released as "public domain". You can copy, modify, re-release and re-license, or incorporate into any other project without restriction of any kind.
//
std.sha256 = function(utf16) {
  function rightRotate(value, amount) {
    return (value >>> amount) | (value << (32 - amount));
  };
  var ascii = std.string2string8(utf16);
  var mathPow = Math.pow;
  var maxWord = mathPow(2, 32);
  var lengthProperty = 'length';
  var i, j; // Used as a counter across the whole file
  var result = '';
  var words = [];
  var asciiBitLength = ascii[lengthProperty] * 8;
  //* caching results is optional - remove/add slash from front of this line to toggle
  // Initial hash value: first 32 bits of the fractional parts of the square roots of the first 8 primes
  // (we actually calculate the first 64, but extra values are just ignored)
  var hash = std.sha256.h = std.sha256.h || [];
  // Round constants: first 32 bits of the fractional parts of the cube roots of the first 64 primes
  var k = std.sha256.k = std.sha256.k || [];
  var primeCounter = k[lengthProperty];
  /*/
  var hash = [], k = [];
  var primeCounter = 0;
  //*/
  var isComposite = {};
  for (var candidate = 2; primeCounter < 64; candidate++) {
    if (!isComposite[candidate]) {
      for (i = 0; i < 313; i += candidate) {
        isComposite[i] = candidate;
      }
      hash[primeCounter] = (mathPow(candidate, .5) * maxWord) | 0;
      k[primeCounter++] = (mathPow(candidate, 1 / 3) * maxWord) | 0;
    }
  }
  ascii += '\x80'; // Append '1' bit (plus zero padding)
  while (ascii[lengthProperty] % 64 - 56) ascii +=
  '\x00'; // More zero padding
  for (i = 0; i < ascii[lengthProperty]; i++) {
    j = ascii.charCodeAt(i);
    if (j >> 8) return; // ASCII check: only accept characters in range 0-255
    words[i >> 2] |= j << ((3 - i) % 4) * 8;
  }
  words[words[lengthProperty]] = ((asciiBitLength / maxWord) | 0);
  words[words[lengthProperty]] = (asciiBitLength)
  // process each chunk
  for (j = 0; j < words[lengthProperty];) {
    var w = words.slice(j, j +=
    16); // The message is expanded into 64 words as part of the iteration
    var oldHash = hash;
    // This is now the "working hash", often labelled as variables a...g
    // (we have to truncate as well, otherwise extra entries at the end accumulate
    hash = hash.slice(0, 8);
    for (i = 0; i < 64; i++) {
      var i2 = i + j;
      // Expand the message into 64 words
      // Used below if
      var w15 = w[i - 15],
        w2 = w[i - 2];
      // Iterate
      var a = hash[0],
        e = hash[4];
      var temp1 = hash[7] + (rightRotate(e, 6) ^ rightRotate(e, 11) ^
          rightRotate(e, 25)) // S1
        + ((e & hash[5]) ^ ((~e) & hash[6])) // ch
        + k[i]
        // Expand the message schedule if needed
        + (w[i] = (i < 16) ? w[i] : (w[i - 16] + (rightRotate(w15, 7) ^
            rightRotate(w15, 18) ^ (w15 >>> 3)) // s0
          + w[i - 7] + (rightRotate(w2, 17) ^ rightRotate(w2, 19) ^ (w2 >>>
            10)) // s1
        ) | 0);
      // This is only used once, so *could* be moved below, but it only saves 4 bytes and makes things unreadble
      var temp2 = (rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a,
          22)) // S0
        + ((a & hash[1]) ^ (a & hash[2]) ^ (hash[1] & hash[2])); // maj
      hash = [(temp1 + temp2) | 0].concat(
      hash); // We don't bother trimming off the extra ones, they're harmless as long as we're truncating when we do the slice()
      hash[4] = (hash[4] + temp1) | 0;
    }
    for (i = 0; i < 8; i++) {
      hash[i] = (hash[i] + oldHash[i]) | 0;
    }
  }
  for (i = 0; i < 8; i++) {
    for (j = 3; j + 1; j--) {
      var b = (hash[i] >> (j * 8)) & 255;
      result += ((b < 16) ? 0 : '') + b.toString(16);
    }
  }
  return result;
};
var theApp = [];
theApp.out = new Out();
theApp.std = std;
theApp.term = new WebTerm(document.getElementById('screen'), window.innerWidth,
  window.innerHeight, theApp.out);
document.theApp = theApp;
theApp.out.term = theApp.term;
//theApp.out.session = std.init(window.location.href + "bin", theApp.out.cb, theApp.out);

function mem2string(ptr, len) {
  const bytes = new Uint8Array(theApp.wasm.exports.memory.buffer, ptr, len);
  return new TextDecoder('utf-8').decode(bytes);
}

function keyboard(e) {
  let seq = e.data;
  if (seq == "\r") {
    seq = "\r\n";
  }

//    const input = seq;
  //  stdinBuffer = Array.from(input + "\n").map(c => c.charCodeAt(0));

  if (seq == "\x1B") {
    //process.exit(0);
  }
  let u = new TextEncoder('utf-8').encode(seq);
  let s = u.length;
  let a = new Uint32Array(theApp.wasm.exports.memory.buffer, theApp.aterm, 6);
  let d = new Uint8Array(theApp.wasm.exports.memory.buffer, theApp.aterm + 4 *
    6, 127);
  if (s > 127) {
    s = 127;
  }
  for (i = 0; i < s; i++) {
    d[i] = u[i]
  }
  d[i] = 0;
  a[3] = 1; /* event type */
  a[4] = s; /* event data length */
  a[5] = theApp.aterm + 4 * 6; /* event data pointer */
  theApp.wasm.exports._event(theApp.aterm);
}

function print(ptr) {
  theApp.term.add_txt(mem2string(ptr));
}

function timeout() {
  let d = new Int32Array(theApp.wasm.exports.memory.buffer, theApp.aterm, 6);
  d[3] = 0;
  d[4] = 0;
  d[5] = 0;
  theApp.wasm.exports._event(theApp.aterm);
  setTimeout(timeout, 100);
}

// Minimal WASI polyfill for stdin/stdout
let stdinBuffer = [];
let stdoutBuffer = "";

// https://wasix.org/docs/api-reference/wasi/
//
const imp = {
  env: {
        _exit: function(rc) {
	},
        _system: function(cmd) {
	},
        _lseek: function(fd, pos, how) {
	},
        _creat: function(path, mode) {
	},
        _truncate: function(path, size) {
	},
        _getdents: function(fd, dirent, count) {
	},
        _access: function(path, mode) {
	},
        _stat: function(path, buf) {
	},
        _mkdir: function(path, mode) {
	},
        _rmdir: function(path) {
	},
        _unlink: function(path) {
	},
        _rename: function(old, new_) {
	},
        _time: function() {
	},
        _write: function(fd,buf,len) {
		theApp.term.add_txt(mem2string(buf, len));
		return len;
	},
        _read: function(fd,buf,len) {
	},
        _close: function(fd) {
	}

  }
};
fetch("test.cmd").then(bytes => bytes.arrayBuffer()).then(mod => WebAssembly
  .compile(mod.slice(128))).then(module => {
  return new WebAssembly.Instance(module, imp);
}).then(instance => {
  theApp.wasm = instance;
  theApp.aterm = instance.exports._init();
  //document.body.addEventListener("input", keyboard);
  setTimeout(timeout, 100);
  theApp._start = instance.exports._start;
});
