#include <karm-main/main.h>

#include "loader.h"

ExitCode entryPoint(CliArgs const &) {
    return Loader::load("/EFI/BOOT/hjert.elf");
}