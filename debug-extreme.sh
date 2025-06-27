#!/bin/bash

CC="gcc -std=c99 -pedantic -Wall -Wextra -Werror \
    -Winline -Wunused-function -Wunused \
    -Wstrict-prototypes -Wold-style-definition \
    -Wmissing-prototypes -Wmissing-declarations \
    -Wredundant-decls -Wnested-externs \
    -Wcast-qual -Wcast-align -Wwrite-strings \
    -Wconversion -Wshadow -Wlogical-op \
    -Waggregate-return -Wstrict-overflow=5 \
    -Wswitch-default -Wswitch-enum \
    -Wunreachable-code -Wfloat-equal \
    \
    -Wformat=2 -Wformat-security -Wformat-nonliteral \
    -Wformat-y2k -Wformat-overflow=2 -Wformat-truncation=2 \
    \
    -Wvla -Wvla-larger-than=1024 \
    -Walloca -Walloc-zero -Walloc-size-larger-than=1048576 \
    \
    -Wduplicated-branches -Wduplicated-cond \
    -Wtrampolines -Wstack-protector \
    -Wunsafe-loop-optimizations \
    \
    -Wbad-function-cast -Wc++-compat \
    -Wjump-misses-init -Wlogical-not-parentheses \
    -Wmissing-field-initializers -Wmissing-include-dirs \
    \
    -Wnull-dereference -Wshift-overflow=2 \
    -Wshift-count-negative -Wshift-count-overflow \
    -Wshift-negative-value \
    \
    -Wstringop-overflow=4 -Wstringop-truncation \
    -Warray-bounds=2 -Wattribute-alias=2 \
    \
    -Wdangling-else -Wdate-time \
    -Wdisabled-optimization -Wdouble-promotion \
    \
    -Winit-self -Winvalid-pch \
    -Wmultichar -Woverlength-strings \
    \
    -Wpacked -Wpacked-bitfield-compat \
    -Wpadded -Wpointer-arith \
    \
    -Wsign-compare -Wsign-conversion \
    -Wstack-usage=8192 \
    \
    -Wswitch-bool -Wsync-nand \
    -Wtautological-compare -Wundef \
    \
    -Wunused-but-set-parameter -Wunused-but-set-variable \
    -Wunused-const-variable=2 -Wunused-local-typedefs \
    -Wunused-macros -Wunused-result \
    \
    -Wvarargs -Wvector-operation-performance \
    -Wvolatile-register-var -Wwrite-strings -Waggregate-return"

# runtime checks flags
CC_DEBUG="$CC \
    -g3 -ggdb -gdwarf-4 \
    -fno-omit-frame-pointer \
    -fsanitize=address,undefined,leak \
    -fstack-protector-strong \
    -D_FORTIFY_SOURCE=2 \
    -O3"

# static analysis flags
CC_STATIC_ANALYSIS="$CC \
    -fanalyzer \
    -Wanalyzer-too-complex \
    -Wanalyzer-null-dereference \
    -Wanalyzer-malloc-leak \
    -Wanalyzer-free-of-non-heap \
    -Wanalyzer-double-free \
    -Wanalyzer-use-after-free"

# experimental flags
CC_EXPERIMENTAL="$CC \
    -Warith-conversion \
    -Wbidi-chars \
    -Wopenacc-parallelism \
    -Wtrivial-auto-var-init"

# Configuração de runtime checks
export MALLOC_CHECK_=2
export MALLOC_PERTURB_=42
export ASAN_OPTIONS=strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1

source install.sh

CC="$CC $CC_DEBUG $CC_STATIC_ANALYSIS $CC_EXPERIMENTAL $CC_RUNTIME_CHECKS"

# clang alternative:
# CC="clang -std=c99 -pedantic-errors   -Wall -Wextra -Werror -Weverything   -Wno-unsafe-buffer-usage   -Wno-padded   -Wno-disabled-macro-expansion   -Wno-reserved-id-macro   -Wno-documentation   -Wstrict-prototypes   -Wcast-align   -Wcast-qual   -Wwrite-strings   -Wbad-function-cast   -Wmissing-prototypes   -Wmissing-declarations   -Wunreachable-code   -Wfloat-equal   -Wshadow   -Wundef   -Wconversion   -Wsign-conversion   -Wno-sign-compare   -Wswitch-enum   -Wcovered-switch-default   -Wstrict-aliasing   -Wredundant-decls   -Wnested-externs   -Wno-newline-eof   -Wdouble-promotion   -Wold-style-definition   -Wshorten-64-to-32   -Wthread-safety   -Wvla   -Wdocumentation-deprecated-sync   -fstrict-aliasing   -fvisibility=hidden   -fsanitize=undefined   -fsanitize=address   -fstack-protector-strong   -fno-common"

# cppcheck test:
# cppcheck   --std=c99   --enable=all   --inconclusive   --force   --inline-suppr   --suppress=missingIncludeSystem   -U__GNUC__ -U__clang__   . -I .

# for now we will use gcc withou flags for libs because unlike bruter and bruter-representation, the libraries are not meant to be 100% portable
export CC="gcc -O3"
bupm install-from localhost:3000/packages io math std os string dycc bit byte condition list alloc type #bsr

valgrind --tool=massif --stacks=yes --detailed-freq=1 --verbose  buxu $1
ms_print massif.out.* > ./massif-out.txt
rm massif.out.*

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file=./valgrind-out.txt \
    --verbose buxu $1