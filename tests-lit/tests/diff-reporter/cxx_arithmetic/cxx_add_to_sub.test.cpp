/**
; RUN: cd / && %CLANG_C_EXEC -fembed-bitcode -g -O0 -DFLAG=1 %p/main.c %p/cxx_add_to_sub.c -o %s.exe
; RUN: cd %CURRENT_DIR
; RUN: %MULL_EXEC -test-framework CustomTest -mutators=cxx_add_to_sub -reporters=Diff %s.exe | %FILECHECK_EXEC %s --strict-whitespace --match-full-lines

CHECK:Survived
CHECK-NEXT:--- a/tests-lit/tests/diff-reporter/cxx_arithmetic/cxx_add_to_sub.c
CHECK-NEXT:+++ b/tests-lit/tests/diff-reporter/cxx_arithmetic/cxx_add_to_sub.c
CHECK-NEXT:@@ -1,4 +1,4 @@
CHECK-NEXT:-int t1(int a, int b) { return a + b; }
CHECK-NEXT:+int t1(int a, int b) { return a - b; }
CHECK-NEXT:
CHECK-NEXT:
CHECK-NEXT:
CHECK:Survived
CHECK-NEXT:--- a/tests-lit/tests/diff-reporter/cxx_arithmetic/cxx_add_to_sub.c
CHECK-NEXT:+++ b/tests-lit/tests/diff-reporter/cxx_arithmetic/cxx_add_to_sub.c
CHECK-NEXT:@@ -12,7 +12,7 @@ void test() {
CHECK-NEXT:
CHECK-NEXT:
CHECK-NEXT:
CHECK-NEXT:-int t2(int a, int b) { return a + b; }
CHECK-NEXT:+int t2(int a, int b) { return a - b; }
CHECK-NEXT:
CHECK-NEXT:
CHECK-NEXT:
CHECK-NEXT:Survived
CHECK-NEXT:--- a/tests-lit/tests/diff-reporter/cxx_arithmetic/cxx_add_to_sub.c
CHECK-NEXT:+++ b/tests-lit/tests/diff-reporter/cxx_arithmetic/cxx_add_to_sub.c
CHECK-NEXT:@@ -21,4 +21,4 @@ int t2(int a, int b) { return a + b; }
CHECK-NEXT:
CHECK-NEXT:
CHECK-NEXT:
CHECK-NEXT:-int t3(int a, int b) { return a + b; }
CHECK-NEXT:+int t3(int a, int b) { return a - b; }
**/

/*

--- a/tests-lit/tests/diff-reporter/cxx_arithmetic/cxx_add_to_sub.c
+++ b/tests-lit/tests/diff-reporter/cxx_arithmetic/cxx_add_to_sub.c
@@ -1,4 +1,4 @@
-int t1(int a, int b) { return a + b; }
+int t1(int a, int b) { return a - b; }



@@ -12,7 +12,7 @@ void test() {



-int t2(int a, int b) { return a + b; }
+int t2(int a, int b) { return a - b; }



@@ -21,4 +21,4 @@ int t2(int a, int b) { return a + b; }



-int t3(int a, int b) { return a + b; }
+int t3(int a, int b) { return a - b; }

*/
