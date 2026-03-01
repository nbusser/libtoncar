#!/bin/bash

# TODO: fix -mthumb-interwork error
# Uses bazel to find the compiler path and add -isystem includes to clang.

compiler_includes=$(bazel query --output=location @devkitarm//:arm-none-eabi/include | cut -d ":" -f1)
compiler_libs=$(bazel query --output=location @devkitarm//:lib | cut -d ":" -f1)

cat > $(dirname "$(realpath $0)")/../../.clang-tidy << EOF
Checks: >
  *,

  -altera-*,
  -android-*,
  -boost-*,
  -darwin-*,
  -fuchsia-*,
  -linuxkernel-*,
  -llvm-*,
  -llvmlibc-*,
  -mpi-*,
  -objc-*,
  -openmp-*,
  -zircon-*,

  -bugprone-crtp-constructor-accessibility,
  -bugprone-easily-swappable-parameters,
  -cppcoreguidelines-avoid-magic-numbers,
  -cppcoreguidelines-avoid-const-or-ref-data-members,
  -cppcoreguidelines-avoid-magic-numbers,
  -cppcoreguidelines-pro-bounds-pointer-arithmetic,
  -cppcoreguidelines-pro-type-reinterpret-cast,
  -cppcoreguidelines-pro-type-vararg,
  -clang-analyzer-cplusplus.NewDelete,
  -google-readability-todo,
  -hicpp-signed-bitwise,
  -hicpp-vararg,
  -misc-no-recursion,
  -modernize-use-trailing-return-type,
  -modernize-use-std-print,
  -performance-enum-size,
  -performance-no-int-to-ptr,
  -readability-identifier-length,
  -readability-magic-numbers,
  -readability-static-accessed-through-instance,
  -performance-enum-size,
  -performance-no-int-to-ptr,

WarningsAsErrors: >
  *,
HeaderFilterRegex: '/src/.+\.h$'
FormatStyle: file
CheckOptions:
  cppcoreguidelines-rvalue-reference-param-not-moved.AllowPartialMove: true
  cppcoreguidelines-avoid-do-while.IgnoreMacros: true
  cppcoreguidelines-pro-type-member-init.UseAssignment: true
  cppcoreguidelines-special-member-functions.AllowSoleDefaultDtor: true
  google-global-names-in-headers.HeaderFileExtensions: ",h,hh,hpp,hxx" # default: h
  google-readability-namespace-comments.SpacesBeforeComments: 2
  misc-non-private-member-variables-in-classes.IgnoreClassesWithAllMemberVariablesBeingPublic: true
  readability-function-cognitive-complexity.IgnoreMacros: true
  readability-function-cognitive-complexity.Threshold: 50
  readability-identifier-length.IgnoredParameterNames: "^(to|h)$"
  readability-identifier-length.IgnoredVariableNames: "^(ok|it|to)$"
  readability-identifier-naming.ClassCase: CamelCase
  readability-identifier-naming.ClassMemberCase: lower_case
  readability-identifier-naming.ConstexprVariableCase: CamelCase
  readability-identifier-naming.ConstexprVariablePrefix: k
  readability-identifier-naming.EnumCase: CamelCase
  readability-identifier-naming.EnumConstantCase: CamelCase
  readability-identifier-naming.FunctionCase: CamelCase
  readability-identifier-naming.FunctionIgnoredRegexp: '^(swap)$'
  readability-identifier-naming.GlobalConstantCase: CamelCase
  readability-identifier-naming.GlobalConstantPrefix: k
  readability-identifier-naming.IgnoreMainLikeFunctions: true
  readability-identifier-naming.MacroDefinitionCase: UPPER_CASE
  readability-identifier-naming.MacroDefinitionIgnoredRegexp: '^[A-Z]+(_[A-Z0-9]+)*_$'
  readability-identifier-naming.MemberCase: lower_case
  readability-identifier-naming.MethodCase: CamelCase
  readability-identifier-naming.MethodIgnoredRegexp: '^(c?begin|c?end|swap)$'
  readability-identifier-naming.NamespaceCase: lower_case
  readability-identifier-naming.ParameterCase: lower_case
  readability-identifier-naming.PrivateMemberSuffix: _
  readability-identifier-naming.PublicMemberSuffix: ''
  readability-identifier-naming.ProtectedMemberSuffix: _
  readability-identifier-naming.StaticConstantCase: CamelCase
  readability-identifier-naming.StaticConstantPrefix: k
  readability-identifier-naming.StructCase: CamelCase
  readability-identifier-naming.TypeAliasCase: CamelCase
  readability-identifier-naming.TypeTemplateParameterCase: CamelCase
  readability-identifier-naming.TypeAliasIgnoredRegexp: '^type$'
  readability-identifier-naming.VariableCase: lower_case
ExtraArgsBefore:
  - -Wno-unknown-argument
  - -nobuiltininc
ExtraArgs:
  - -isystem$compiler_includes/c++/12.1.0
  - -isystem$compiler_includes/c++/12.1.0/arm-none-eabi
  - -isystem$compiler_includes/c++/12.1.0/arm-none-eabi/12.1.0
  - -isystem$compiler_libs/gcc/arm-none-eabi/12.1.0/include
  - -isystem$compiler_libs/gcc/arm-none-eabi/12.1.0/include-fixed
  - -isystem$compiler_includes
EOF
