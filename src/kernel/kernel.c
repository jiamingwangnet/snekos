void kernel_main()
{
    for(int i = 0; i < 200; i++)
    *((int*)0xb8000 + i)=0x1d691d48;
}