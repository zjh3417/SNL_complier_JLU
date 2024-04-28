#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
long snl_syscall(long number, ...);

// i686-linux-gnu-gcc ./snllib.c -masm=intel -fno-stack-protector -c -o snllib.o
asm(
    ".global snl_syscall\n\t"
    "snl_syscall:\n\t"
    "push    ebp\n\t"
    "push    edi\n\t"
    "push    esi\n\t"
    "push    ebx\n\t"
    "mov     ebp, [esp + 0x14 + 0x18]\n\t"
    "mov     edi, [esp + 0x14 + 0x14]\n\t"
    "mov     esi, [esp + 0x14 + 0x10]\n\t"
    "mov     edx, [esp + 0x14 + 0xC]\n\t"
    "mov     ecx, [esp + 0x14 + 0x8]\n\t"
    "mov     ebx, [esp + 0x14 + 0x4]\n\t"
    "mov     eax, [esp + 0x14 + 0x0]\n\t"
    "int     0x80\n\t"
    "pop     ebx\n\t"
    "pop     esi\n\t"
    "pop     edi\n\t"
    "pop     ebp\n\t"
    "ret\n\t"
);


asm(
    ".global find_param_var_address\n\t"
    "find_param_var_address:\n\t"
    "mov eax, [esp + 0x4];\n\t"
    "lea esi, [esp + 0xC]\n\t"
    "mov edx, ebp\n\t"
    "find_param_var_address_loop:\n\t"
    "cmp eax, [esi + 0x04]\n\t"
    "jz found_param_var_address\n\t"
    "mov esi, [esi]\n\t"
    "mov edx, [edx]\n\t"
    "jmp find_param_var_address_loop\n\t"
    "found_param_var_address:\n\t"
    "lea eax, [edx + 0x8]\n\t"
    "add eax, [esp + 0x8]\n\t"
    "ret\n\t"
);


asm(
    ".global find_local_var_address\n\t"
    "find_local_var_address:\n\t"
    "mov eax, [esp + 0x4];\n\t"
    "lea esi, [esp + 0xC]\n\t"
    "find_local_var_address_loop:\n\t"
    "cmp eax, [esi + 0x04]\n\t"
    "jz found_local_var_address\n\t"
    "mov esi, [esi]\n\t"
    "jmp find_local_var_address_loop\n\t"
    "found_local_var_address:\n\t"
    "lea eax,[esi + 0x8]\n\t"
    "add eax,[esp + 0x8]\n\t"
    "ret\n\t"
);

asm(
    ".global _start\n\t"
    "_start:\n\t"
    "call procedure_0\n\t"
    "push 0\n\t"
    "push 1\n\t"
    "call snl_syscall\n\t"
    "hlt\n\t"
);

void out_char(char ch,char end){
    snl_syscall(SYS_write,1,&ch,1);
    if(end)
        snl_syscall(SYS_write,1,&end,1);
}

void in_char(char * ch){
    snl_syscall(SYS_read,0,ch,1);
}

void print_char(char ch){
    out_char(ch,'\n');
}

void print_int(int n){
    char str[0x100];
    int l = 0;
    int sign = 0;

    if(n == 0){
        out_char('0','\n');
        return;
    }

    if(n < 0){
        n = -n;
        sign = 1;
    }

    while(n){
        str[l++] = (n % 10) + '0';
        n = n / 10;
    };
    str[l] = 0;
    --l;
    
    if(sign){
        out_char('-',0);
    }

    while(l >= 0){
        out_char(str[l],0);
        l--;
    }
    out_char('\n',0);
}

void print_char_array(char* ch_arr,int size){
    for(int i = 0;i<size;i++){
        out_char(ch_arr[i],0);
    }
    out_char('\n',0);
}

void print_int_array(int * int_arr,int size){
    for(int i = 0;i < size;i++){
        char str[0x100];
        int l = 0;
        int n = int_arr[i];

        if(n == 0){
            out_char('0',' ');
            continue;
        }

        while(n){
            str[l++] = (n % 10) + '0';
            n = n / 10;
        };
        str[l] = 0;
        --l;
        while(l >= 0){
            out_char(str[l],0);
            l--;
        }
        out_char(' ',0);
    }
    out_char('\n',0);
}

char in_buff[0x1000];

void getline(){
    char ch = 0;
    int l = 0;

    while (1){
        in_char(&ch);
        if(ch == '\n'){
            break;
        }

        in_buff[l++] = ch;
    }
    in_buff[l] = 0;
}


void read_char(char * ch){
    getline();
    *ch = in_buff[0];
}

void read_integer(int * num){
    getline();
    int n = 0;
    int sign = 0;
    char * p = in_buff;

    if(in_buff[0] == '-'){
        sign = 1;
        ++p;
    }

    while(*p >= '0' && *p <= '9'){
        n = n * 10 + (*p) - '0';
        ++p;
    }

    if(sign){
        n = -n;
    }

    *num = n;
}