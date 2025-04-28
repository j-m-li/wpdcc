/* "$Id: $"
 *
 *                          9 December MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 *
 * 
 */

#include <lang/Object.h>
#include <io/Folder.h>
#include <io/File.h>
#include <lang/Process.h>

void dump_result(Ex **e__)
{
	F("check_result");
	io__File *f;
	String fn;
	String mode;
	String *out;

	ECHO("============================================\n");
	STR(mode, "exist");
	STR(fn, "result.txt");	
	f = new_(io__File);
	CHECK(error1);
	f->_->open(e__, f, &fn, &mode);
	CHECK(error2);
	out = new_(String);
	CHECK(error2);
	while (f->_->end_of_file(e__, f) == 0) {
		f->_->read_line(e__, f, out);
		CHECK(error3);
		echo(out);
		out->_->clear(e__, out);
		CHECK(error3);
	}
	delete_(out);
	CHECK(error2);
	delete_(f);
	CHECK(error1);
	ECHO("============================================\n");
	return;
error3:
	delete_(out);
error2:
	delete_(f);
error1:
	return;
}

int32 is_ok(Ex **e__, String *out, utf8 **error_list)
{
	int32 i = 0;
	String s;
	String s1;
	
	while (error_list[i][0] != 0) {
		STR(s, error_list[i]);
		if (out->_->index_of(e__, out, &s) >= 0) {
			return 1;
		}
		i++;
	}

	STR(s, "E-");
	STR(s1, ":");
	if (out->_->index_of(e__, out, &s) >= 0 &&
		out->_->index_of(e__, out, &s1) >= 0) 
	{
			return 0;
	}
	return 1;
}

int32 check_result(Ex **e__, utf8 **error_list)
{
	F("check_result");
	io__File *f;
	String fn;
	String mode;
	String *out;
	int32 r = 1;

	STR(mode, "exist");
	STR(fn, "result.txt");	
	f = new_(io__File);
	CHECK(error1);
	f->_->open(e__, f, &fn, &mode);
	CHECK(error2);
	out = new_(String);
	CHECK(error2);
	while (f->_->end_of_file(e__, f) == 0) {
		f->_->read_line(e__, f, out);
		CHECK(error3);

		if (is_ok(e__, out, error_list) != 1) {
			r = 0;
		} 
		out->_->clear(e__, out);
		CHECK(error3);
	}
	delete_(out);
	CHECK(error2);
	delete_(f);
	CHECK(error1);
	return r;
error3:
	delete_(out);
error2:
	delete_(f);
error1:
	return 0;
}

void get_error_list(Ex **e__, String *fn, utf8 **error_list)
{
	F("get_error_list");
	io__File *f;
	String mode;
	String *out;
	int32 i = 0;

	error_list[i][0] = 0;
	STR(mode, "exist");
	f = new_(io__File);
	CHECK(error1);
	f->_->open(e__, f, fn, &mode);
	CHECK(error2);
	out = new_(String);
	CHECK(error2);
	while (f->_->end_of_file(e__, f) == 0) {
		utf8 *p;
		f->_->read_line(e__, f, out);
		CHECK(error3);
		p = out->cstr;
		if (p != 0 && p[0] == '*' && p[1] == '/') break;
		if (p != 0 && (p[0] == '/' && p[1] == '*') == 0 && p[0] == 'E') {
			int32 j = 0;
			while (p[j] != 0 && p[j] != '\r' && p[j] != '\n' &&
				p[j] != '\t' && p[j] != ' ')
			{
				error_list[i][j] = p[j];
				j++;
			}
			error_list[i][j] = 0;
			i++;
			error_list[i][0] = 0;
		}
		out->_->clear(e__, out);
		CHECK(error3);
	}
	delete_(out);
	CHECK(error2);
	delete_(f);
	CHECK(error1);
	return;
error3:
	delete_(out);
error2:
	delete_(f);
error1:
	return;

}

int32 run(Ex **e__, String *f, String *path)
{
	F("run");
	String c;
	String *cmd;
	Process *p;
	utf8 *error_list[20];
	utf8 el[10][12];
	int32 i;

	for (i = 0; i < (sizeof(el) / sizeof(el[0])); i++) {
		error_list[i] = &el[i][0];
	}
	cmd = new_(String);
	CHECK(error1);

	cmd->_->add(e__, cmd, path);
	cmd->_->add(e__, cmd, f);
	echo(cmd);
	get_error_list(e__, cmd, error_list);
	CHECK(error2);

	cmd->_->clear(e__, cmd);
	STR(c, "cmd /c \"..\\..\\bin\\cl.exe ");
	cmd->_->add(e__, cmd, &c);
	cmd->_->add(e__, cmd, path);
	cmd->_->add(e__, cmd, f);
	STR(c, " >result.txt\"");
	cmd->_->add(e__, cmd, &c);
	CHECK(error2);
	p = new_(Process);
	CHECK(error2);
	//echo(cmd);
	p->_->create(e__, p, cmd);
	CHECK(error3);
	p->_->wait_for(e__, p, 0xFFFFFFFF);
	CHECK(error3);
	delete_(p);
	CHECK(error2);
	delete_(cmd);
	CHECK(error1);
	return check_result(e__, error_list);
error3:
	delete_(p);
error2:
	delete_(cmd);
error1:
	return 0;
}

void wpdcc_main(Ex **e__)
{
	F("wpdcc_main");
	io__Folder *f;
	int32 pass_failed = 0;
	int32 pass_ok = 0;
	int32 error_failed = 0;
	int32 error_ok = 0;
	int32 warn_failed = 0;
	int32 warn_ok = 0;

	String s;
	String *fi;
	String mode;
	ECHO("running WPDCC tests...\n");
	CHECK(error1);
	f = new_(io__Folder);
	CHECK(error1);
	fi = new_(String);
	CHECK(error2);
	ECHO("../../test/cl/pass/*\n");
	STR(s, "../../test/cl/pass/*.c");
	STR(mode, "exist");
	f->_->open(e__, f, &s, &mode);
	CHECK(error3);

	while (f->_->find_next_file(e__, f, fi) != 0) {
		int32 r;
		String p;

		STR(p, "../../test/cl/pass/");
		r = run(e__, fi, &p);
		fi->_->clear(e__, fi);
		CHECK(error3);
		if (r == 0) {
			pass_failed++;
			ECHO(" FAIL\n");
			dump_result(e__);
			CHECK(error3);
		} else {
			pass_ok++;
			ECHO(" OK\n");	
		}
	}
	f->_->close(e__, f);
	CHECK(error3);

#if 0
	ECHO("../../test/cl/warn/*\n");
	STR(s, "../../test/cl/warn/*.c");
	STR(mode, "exist");
	f->_->open(e__, f, &s, &mode);
	CHECK(error3);

	while (f->_->find_next_file(e__, f, fi) != 0) {
		int32 r;
		String p;

		STR(p, "../../test/cl/warn/");
		r = run(e__, fi, &p);
		fi->_->clear(e__, fi);
		CHECK(error3);
		if (r == 0) {
			warn_failed++;
			ECHO(" FAIL\n");
			dump_result(e__);
			CHECK(error3);
		} else {
			warn_ok++;
			ECHO(" OK\n");	
		}
	}
	f->_->close(e__, f);
	CHECK(error3);


	ECHO("../../test/cl/error/*\n");
	STR(s, "../../test/cl/error/*.c");
	STR(mode, "exist");
	f->_->open(e__, f, &s, &mode);
	CHECK(error3);

	while (f->_->find_next_file(e__, f, fi) != 0) {
		int32 r;
		String p;

		STR(p, "../../test/cl/error/");
		r = run(e__, fi, &p);
		fi->_->clear(e__, fi);
		CHECK(error3);
		if (r == 0) {
			error_failed++;
			ECHO(" FAIL\n");
			dump_result(e__);
			CHECK(error3);
		} else {
			error_ok++;
			ECHO(" OK\n");	
		}
	}
	f->_->close(e__, f);
	CHECK(error3);
#endif

	if (pass_failed == 0 && warn_failed == 0 && error_failed == 0) {
		ECHO("\n\nWPDCC tests passed...\n");
	} else {
		ECHO("\n\nWPDCC tests FAILED...\n");
	}
	system("pause");
	delete_(fi);
	CHECK(error2);
	delete_(f);
	CHECK(error1);
	return;
error3:
	delete_(fi);
error2:
	delete_(f);
error1:
	echo((*e__)->stack);
	system("pause");
	return;
}
