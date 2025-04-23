#define NOB_IMPLEMENTATION
#include "nob.h"

bool build_and_run_o_file(Nob_Cmd *cmd, const char* output, const char* input) {
    nob_cmd_append(cmd, "g++", "-c", "-I./include", "-I.", "-o", output, input);
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;

    return true;
}

bool make_o_files_from_dir(const char* dir, Nob_Cmd *cmd, Nob_String_View *sv, Nob_File_Paths* nfp) {
    if(!nob_read_entire_dir(dir, nfp)) return false;
    nob_da_foreach(const char *, it, nfp) {
        sv->count = 0;
        *sv = nob_sv_from_cstr(*it);
        if(**it != '.' && nob_sv_end_with(*sv, ".cpp")) {
            const char * input = nob_temp_sprintf("%s/" SV_Fmt, dir, SV_Arg(*sv));
            const char * output = nob_temp_sprintf("build/nob/" SV_Fmt ".o", SV_Arg(*sv));
            if (nob_needs_rebuild1(output, input)) {
              if (!build_and_run_o_file(cmd, output, input)) return false;
            }
        }
    }
    nfp->count = 0;
    return true;
}

bool build(Nob_Cmd *cmd, Nob_String_View *sv, Nob_File_Paths* nfp) {
    if(!nob_read_entire_dir("build/nob", nfp)) return false;
    const char * output = "build/emulator";
    nob_cmd_append(cmd, "g++", "-o", output);
    nob_da_foreach(const char *, it, nfp) {
        sv->count = 0;
        *sv = nob_sv_from_cstr(*it);
        if(**it != '.' && nob_sv_end_with(*sv, ".o")) {
            const char * input = nob_temp_sprintf("build/nob/" SV_Fmt, SV_Arg(*sv));
            nob_cmd_append(cmd, input);
        }
    }
    nob_cmd_append(cmd,
        "-Lthird_party/Raylib/raylib-5.0_linux_amd64/lib",
        "-Lthird_party/tinyfiledialogs/dll_cs_lua_R_fortran_pascal",
        "-lm",
        "-ldl",
        "-l:libraylib.a",
        "-l:tinyfiledialogsLinux64.a",
    );
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    nfp->count = 0;
    return true;
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    nob_mkdir_if_not_exists("build");
    nob_mkdir_if_not_exists("build/nob");
    Nob_Cmd cmd = {0};
    Nob_String_View sv = {0};
    Nob_File_Paths nfp = {0};
    if(!make_o_files_from_dir("src/Source_files", &cmd, &sv, &nfp)) return 1;
    if(!make_o_files_from_dir("src/Mappers", &cmd, &sv, &nfp)) return 1;
    if(!make_o_files_from_dir("src", &cmd, &sv, &nfp)) return 1;

    if(!build(&cmd, &sv, &nfp)) return 1;

    return 0;
}

// TODO: Make the .h files trigger rebuild
