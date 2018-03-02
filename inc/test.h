#pragma once
#include <stdint.h>

#if defined _MSC_VER && defined _DEBUG
#include <crtdbg.h>
#define BREAK() (_CrtDbgBreak(),0)
#else
#include <stdlib.h>
#define BREAK() (abort(),0)
#endif

typedef struct str str_t;
typedef struct logger log_t;

typedef int(*test_failure_fn)(const char*, int, const char*);
extern test_failure_fn test_failed;

log_t *start_test(int *argc, char *argv[], int timeout_ms);
int finish_test();

int assert_true(int a, const char *astr, const char *file, int line);
int expect_int_eq(int64_t a, int64_t b, const char *astr, const char *bstr, const char *file, int line);
int expect_int_gt(int64_t a, int64_t b, const char *astr, const char *bstr, const char *file, int line);
int expect_int_ge(int64_t a, int64_t b, const char *astr, const char *bstr, const char *file, int line);
int expect_str_eq(const char *a, const char *b, const char *astr, const char *bstr, const char *file, int line);
int expect_bytes_eq(const void *a, int alen, const void *b, int blen, const char *astr, const char *bstr, const char *file, int line);
int expect_float_eq(double a, double b, const char *astr, const char *bstr, const char *file, int line);
int expect_near(double a, double b, double delta, const char *astr, const char *bstr, const char *file, int line);
int expect_ptr_eq(const void *a, const void *b, const char *astr, const char *bstr, const char *file, int line);

int is_equiv_float(double a, double b);
void print_test_data(str_t *s, const uint8_t *a, int sz);

#define ASSERT_TRUE(A) (assert_true((A), #A, __FILE__, __LINE__) && BREAK())
#define EXPECT_EQ(A, B) (expect_int_eq((A), (B), #A, #B, __FILE__, __LINE__) && BREAK())
#define EXPECT_GT(A, B) (expect_int_gt((A), (B), #A, #B, __FILE__, __LINE__) && BREAK())
#define EXPECT_GE(A, B) (expect_int_ge((A), (B), #A, #B, __FILE__, __LINE__) && BREAK())
#define EXPECT_STREQ(A, B) (expect_str_eq((A), (B), #A, #B, __FILE__, __LINE__) && BREAK())
#define EXPECT_BYTES_EQ(A, ALEN, B, BLEN) (expect_bytes_eq((A), (ALEN), (B), (BLEN), #A, #B, __FILE__, __LINE__) && BREAK())
#define EXPECT_FLOAT_EQ(A, B) (expect_float_eq((A), (B), #A, #B, __FILE__, __LINE__) && BREAK())
#define EXPECT_NEAR(A, B, DELTA) (expect_near((A), (B), (DELTA), #A, #B, __FILE__, __LINE__) && BREAK())
#define EXPECT_PTREQ(A, B) (expect_ptr_eq((A), (B), #A, #B, __FILE__, __LINE__) && BREAK())
