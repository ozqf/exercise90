#pragma once

#include "../common/com_module.cpp"

/**
 * Just some random stuff
 */

void Test_WriteRandomNumberTable(char* fileName, i32 count, i32 seed)
{
    FILE* f;
    fopen_s(&f, fileName, "w");
    if (f == NULL)
    {
        printf("  Failed to create file %s\n", fileName);
        return;
    }
    fprintf(f, "#pragma once\n\n#define COM_RANDOM_NUMBER_TABLE_SIZE %d\n\nstatic f32 com_random[COM_RANDOM_NUMBER_TABLE_SIZE] = [\n", count);
    //char buffer[64];
    i32 written = 0;
    //for (i32 i = 0; i < count; ++i)
    for (;;)
    {
        f32 value = COM_STDRandf32();
        //written = sprintf_s(buffer, 64, "%f,\n", value);
        //fwrite(buffer, written, 1, f);
        
        written++;
        if (written < count)
        {
            fprintf(f, "\t%f,\n", value);
        }
        else
        {
            fprintf(f, "\t%f\n];\n\n", value);
            break;
        }
    }
    i32 pos = ftell(f);

    printf("Wrote %d characters to %s\n", pos, fileName);
    fclose(f);
}

void Test_RandomNumbers()
{

    i32 seed = 10;
    i32 count = 0;
    while(count++ < 10)
    {
        printf("Seed %d %f\n", seed, COM_Randf32(&seed));
    }
}
