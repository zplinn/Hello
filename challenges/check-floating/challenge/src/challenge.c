#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <setjmp.h>
#include <dlfcn.h>
#include <math.h>

struct account_t
{
    float balance;
};

struct bank_t
{
    struct account_t accounts[8];
    int n_accounts;
    int owned;
};

uint64_t read_int(void)
{
    uint64_t value;
    scanf("%" SCNu64, &value);
    return value;
}

float read_float(void)
{
    float value;
    scanf("%f", &value);
    return value;
}

void init(void)
{
    setvbuf(stdin, NULL, _IOFBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main(int argc, char** argv, char** envp)
{
    init();

    struct bank_t bank;
    memset(&bank, 0, sizeof(struct bank_t));
    bank.n_accounts = 0;
    bank.owned = 0;
    
    while (1)
    {
        printf("Bank of ctf\n");
        printf("%d accounts:\n", bank.n_accounts);
        for (int i = 0; i < bank.n_accounts; i ++)
        {
            printf("  Account %d: $%.02f\n", i, bank.accounts[i].balance);
        }

        printf("\n");
        printf("Menu: \n");
        printf("1. Open account\n");
        printf("2. Do transactions\n");
        printf("3. Purchase the bank\n");
        if (bank.owned)
        {
            printf("4. Get a shell\n");
        }

        printf("Choice: ");

        switch (read_int())
        {
            case 1:
            {
                if (bank.n_accounts == 8)
                {
                    printf("Bank manager could not be found to open a new account\n");
                    printf("Try using the accounts you already have :)\n");
                    break;
                }

                printf("Opened account %d for you! As a bonus, we'll give you $100!\n", bank.n_accounts);
                bank.accounts[bank.n_accounts].balance = 100;
                bank.n_accounts += 1;
                break;
            }
            case 2:
            {
                printf("Which account is sending: ");
                int sender = read_int();
                if (sender < 0 || sender >= bank.n_accounts)
                {
                    printf("Unknown sender!\n");
                    break;
                }

                printf("Which account is receiving: ");
                int receiver = read_int();
                if (receiver < 0 || receiver >= bank.n_accounts)
                {
                    printf("Unknown receiver!\n");
                    break;
                }

                printf("How much money: ");
                float amount = read_float();

                if (!isfinite(amount))
                {
                    printf("Can't send infinite money\n");
                    break;
                }

                printf("How many times: ");
                int times = read_int();

                if (amount * (float)times <= bank.accounts[sender].balance)
                {
                    for (int i = 0; i < times; i ++)
                    {
                        bank.accounts[sender].balance -= amount;
                        bank.accounts[receiver].balance += amount;
                    }
                }
                else
                {
                    printf("Sender doesn't have enough money!\n");
                }

                break;
            }
            case 3:
            {
                printf("Want to buy the bank? Sure! That will be $1000000!\n");
                printf("Which account is buying the bank: ");
                int buyer = read_int();
                if (buyer < 0 || buyer >= bank.n_accounts)
                {
                    printf("Unknown buyer!\n");
                    break;
                }

                if (bank.accounts[buyer].balance >= 1000000.0f)
                {
                    bank.accounts[buyer].balance -= 1000000.0f;
                    bank.owned = 1;
                }
                else
                {
                    printf("Buyer doesn't have enough money!\n");
                }
                break;
            }
            case 4:
            {
                if (!bank.owned)
                {
                    printf("You don't own the bank yet!\n");
                    break;
                }
                system("/bin/bash");
                return 0;
            }
            default:
            {
                return 0;
            }
        }
    }

    
    return 0;
}
