#define panic(x) (void)0
/* We need to provide these functions never call them */
void __aeabi_unwind_cpp_pr0(void);
void __aeabi_unwind_cpp_pr1(void);
void __aeabi_unwind_cpp_pr2(void);

void __aeabi_unwind_cpp_pr0(void)
{
    panic("__aeabi_unwind_cpp_pr0");
}

void __aeabi_unwind_cpp_pr1(void)
{
    panic("__aeabi_unwind_cpp_pr1");
}

void __aeabi_unwind_cpp_pr2(void)
{
    panic("__aeabi_unwind_cpp_pr2");
}

