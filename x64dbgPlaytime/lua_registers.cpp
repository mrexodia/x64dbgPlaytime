#include "lua_context.hpp"
#include "lauxlib.h"
#include "pluginsdk/bridgemain.h"
#include "plugin.h"

#include <windows.h>
#include <unordered_map>
#include <stdint.h>
#include <string>
#include <functional>
#include <algorithm>

struct RegisterData_t
{
    size_t offset;
    size_t readSize;
    size_t writeSize;
};

#define REGISTER_RECORD(name, reg, offset, readSize, writeSize) \
    { name, { offsetof(CONTEXT, ##reg##) + offset, readSize, writeSize} }

static const std::unordered_map<std::string, RegisterData_t> RegisterOffsetMap =
{
#ifdef _WIN64
    REGISTER_RECORD("rax", Rax, 0, 64, 64),
    REGISTER_RECORD("eax", Rax, 0, 32, 64),
    REGISTER_RECORD("ax", Rax, 0, 16, 16),
    REGISTER_RECORD("ah", Rax, 1, 8, 8),
    REGISTER_RECORD("al", Rax, 0, 8, 8),

    REGISTER_RECORD("rbx", Rbx, 0, 64, 64),
    REGISTER_RECORD("ebx", Rbx, 0, 32, 64),
    REGISTER_RECORD("bx", Rbx, 0, 16, 16),
    REGISTER_RECORD("bh", Rbx, 1, 8, 8),
    REGISTER_RECORD("bl", Rbx, 0, 8, 8),

    REGISTER_RECORD("rcx", Rcx, 0, 64, 64),
    REGISTER_RECORD("ecx", Rcx, 0, 32, 64),
    REGISTER_RECORD("cx", Rcx, 0, 16, 16),
    REGISTER_RECORD("ch", Rcx, 1, 8, 8),
    REGISTER_RECORD("cl", Rcx, 0, 8, 8),

    REGISTER_RECORD("rdx", Rdx, 0, 64, 64),
    REGISTER_RECORD("edx", Rdx, 0, 32, 64),
    REGISTER_RECORD("dx", Rdx, 0, 16, 16),
    REGISTER_RECORD("dh", Rdx, 1, 8, 8),
    REGISTER_RECORD("dl", Rdx, 0, 8, 8),

    REGISTER_RECORD("rdi", Rdi, 0, 64, 64),
    REGISTER_RECORD("edi", Rdi, 0, 32, 64),
    REGISTER_RECORD("di", Rdi, 0, 16, 16),
    REGISTER_RECORD("dil", Rdi, 0, 8, 8),

    REGISTER_RECORD("rsi", Rsi, 0, 64, 64),
    REGISTER_RECORD("esi", Rsi, 0, 32, 64),
    REGISTER_RECORD("si", Rsi, 0, 16, 16),
    REGISTER_RECORD("sil", Rsi, 0, 8, 8),

    REGISTER_RECORD("rbp", Rbp, 0, 64, 64),
    REGISTER_RECORD("ebp", Rbp, 0, 32, 64),
    REGISTER_RECORD("bp", Rbp, 0, 16, 16),
    REGISTER_RECORD("bpl", Rbp, 0, 8, 8),

    REGISTER_RECORD("rsp", Rsp, 0, 64, 64),
    REGISTER_RECORD("esp", Rsp, 0, 32, 64),
    REGISTER_RECORD("sp", Rsp, 0, 16, 16),
    REGISTER_RECORD("spl", Rsp, 0, 8, 8),

    REGISTER_RECORD("rip", Rip, 0, 64, 64),
    REGISTER_RECORD("eip", Rip, 0, 32, 64),

    REGISTER_RECORD("r8", R8, 0, 64, 64),
    REGISTER_RECORD("r8d", R8, 0, 32, 64),
    REGISTER_RECORD("r8w", R8, 0, 16, 16),
    REGISTER_RECORD("r8b", R8, 0, 8, 8),

    REGISTER_RECORD("r9", R9, 0, 64, 64),
    REGISTER_RECORD("r9d", R9, 0, 32, 64),
    REGISTER_RECORD("r9w", R9, 0, 16, 16),
    REGISTER_RECORD("r9b", R9, 0, 8, 8),

    REGISTER_RECORD("r10", R10, 0, 64, 64),
    REGISTER_RECORD("r10d", R10, 0, 32, 64),
    REGISTER_RECORD("r10w", R10, 0, 16, 16),
    REGISTER_RECORD("r10b", R10, 0, 8, 8),

    REGISTER_RECORD("r11", R11, 0, 64, 64),
    REGISTER_RECORD("r11d", R11, 0, 32, 64),
    REGISTER_RECORD("r11w", R11, 0, 16, 16),
    REGISTER_RECORD("r11b", R11, 0, 8, 8),

    REGISTER_RECORD("r12", R12, 0, 64, 64),
    REGISTER_RECORD("r12d", R12, 0, 32, 64),
    REGISTER_RECORD("r12w", R12, 0, 16, 16),
    REGISTER_RECORD("r12b", R12, 0, 8, 8),

    REGISTER_RECORD("r13", R13, 0, 64, 64),
    REGISTER_RECORD("r13d", R13, 0, 32, 64),
    REGISTER_RECORD("r13w", R13, 0, 16, 16),
    REGISTER_RECORD("r13b", R13, 0, 8, 8),

    REGISTER_RECORD("r14", R14, 0, 64, 64),
    REGISTER_RECORD("r14d", R14, 0, 32, 64),
    REGISTER_RECORD("r14w", R14, 0, 16, 16),
    REGISTER_RECORD("r14b", R14, 0, 8, 8),

    REGISTER_RECORD("r15", R15, 0, 64, 64),
    REGISTER_RECORD("r15d", R15, 0, 32, 64),
    REGISTER_RECORD("r15w", R15, 0, 16, 16),
    REGISTER_RECORD("r15b", R15, 0, 8, 8),

    REGISTER_RECORD("eflags", EFlags, 0, 32, 32),
    REGISTER_RECORD("rflags", EFlags, 0, 32, 32),
#else
    REGISTER_RECORD("eax", Eax, 0, 32, 32),
    REGISTER_RECORD("ax", Eax, 0, 16, 16),
    REGISTER_RECORD("ah", Eax, 1, 8, 8),
    REGISTER_RECORD("al", Eax, 0, 8, 8),

    REGISTER_RECORD("ebx", Ebx, 0, 32, 32),
    REGISTER_RECORD("bx", Ebx, 0, 16, 16),
    REGISTER_RECORD("bh", Ebx, 1, 8, 8),
    REGISTER_RECORD("bl", Ebx, 0, 8, 8),

    REGISTER_RECORD("ecx", Ecx, 0, 32, 32),
    REGISTER_RECORD("cx", Ecx, 0, 16, 16),
    REGISTER_RECORD("ch", Ecx, 1, 8, 8),
    REGISTER_RECORD("cl", Ecx, 0, 8, 8),

    REGISTER_RECORD("edx", Edx, 0, 32, 32),
    REGISTER_RECORD("dx", Edx, 0, 16, 16),
    REGISTER_RECORD("dh", Edx, 1, 8, 8),
    REGISTER_RECORD("dl", Edx, 0, 8, 8),

    REGISTER_RECORD("edi", Edi, 0, 32, 32),
    REGISTER_RECORD("di", Edi, 0, 16, 16),
    REGISTER_RECORD("dil", Edi, 0, 8, 8),

    REGISTER_RECORD("esi", Esi, 0, 32, 32),
    REGISTER_RECORD("si", Esi, 0, 16, 16),
    REGISTER_RECORD("sil", Esi, 0, 8, 8),

    REGISTER_RECORD("ebp", Ebp, 0, 32, 32),
    REGISTER_RECORD("bp", Ebp, 0, 16, 16),
    REGISTER_RECORD("bpl", Ebp, 0, 8, 8),

    REGISTER_RECORD("esp", Esp, 0, 32, 32),
    REGISTER_RECORD("sp", Esp, 0, 16, 16),
    REGISTER_RECORD("spl", Esp, 0, 8, 8),

    REGISTER_RECORD("eip", Eip, 0, 32, 32),

    REGISTER_RECORD("eflags", EFlags, 0, 32, 32),
#endif
};

static uint64_t readRegister(const CONTEXT& ctx, const RegisterData_t& regData)
{
    const uint8_t *ctxBase = reinterpret_cast<const uint8_t*>(&ctx);

    switch (regData.readSize)
    {
    case 64:
        return *(uint64_t*)(ctxBase + regData.offset);
    case 32:
        return *(uint32_t*)(ctxBase + regData.offset);
    case 16:
        return *(uint16_t*)(ctxBase + regData.offset);
    case 8:
        return *(uint8_t*)(ctxBase + regData.offset);
    }

    return 0;
}

static void writeRegister(CONTEXT& ctx, const RegisterData_t& regData, uint64_t value)
{
    uint8_t *ctxBase = reinterpret_cast<uint8_t*>(&ctx);

    switch (regData.writeSize)
    {
    case 64:
        *(uint64_t*)(ctxBase + regData.offset) = value;
        break;
    case 32:
        *(uint32_t*)(ctxBase + regData.offset) = (uint32_t)value;
        break;
    case 16:
        *(uint16_t*)(ctxBase + regData.offset) = (uint16_t)value;
        break;
    case 8:
        *(uint8_t*)(ctxBase + regData.offset) = (uint8_t)value;
        break;
    }
}

static int lua_register_read(lua_State *L)
{
    if (DbgIsDebugging() == false)
    {
        lua_pushnil(L);
        return 1;
    }

    if (lua_isstring(L, 1))
    {
        std::string regName = lua_tostring(L, 1);
        std::transform(regName.begin(), regName.end(), regName.begin(), tolower);

        auto regData = RegisterOffsetMap.find(regName);
        if (regData == RegisterOffsetMap.end())
        {
            lua_pushnil(L);
            return 1;
        }

        HANDLE hThread = DbgGetThreadHandle();

        CONTEXT ctx;
        ctx.ContextFlags = CONTEXT_FULL;
        if (GetThreadContext(hThread, &ctx) == FALSE)
        {
            DWORD lastError = GetLastError();
            _plugin_logprintf("ERROR: Unable to get thread context: %08X\n", lastError);
            return 0;
        }

        lua_pushinteger(L, readRegister(ctx, regData->second));
    }
    // TODO: Check if thread object and read from there.

    return 1;
}

static int lua_register_write(lua_State *L)
{
    if (DbgIsDebugging() == false)
    {
        return 0;
    }

    if (lua_isstring(L, 1) && lua_isinteger(L, 2))
    {
        std::string regName = lua_tostring(L, 1);
        std::transform(regName.begin(), regName.end(), regName.begin(), tolower);

        const auto regData = RegisterOffsetMap.find(regName);
        if (regData == RegisterOffsetMap.end())
        {
            lua_pushnil(L);
            return 1;
        }

        uint64_t val = lua_tointeger(L, 2);

        HANDLE hThread = DbgGetThreadHandle();

        CONTEXT ctx;
        ctx.ContextFlags = CONTEXT_FULL;
        if (GetThreadContext(hThread, &ctx) == FALSE)
        {
            DWORD lastError = GetLastError();
            _plugin_logprintf("ERROR: Unable to get thread context: %08X\n", lastError);
            return 0;
        }

        if (readRegister(ctx, regData->second) != val)
        {
            writeRegister(ctx, regData->second, val);

            if (SetThreadContext(hThread, &ctx) == FALSE)
            {
                DWORD lastError = GetLastError();
                _plugin_logprintf("ERROR: Unable to set thread context: %08X\n", lastError);
                return 0;
            }

            GuiUpdateRegisterView();
        }
    }
    // TODO: Check if thread object and read from there.

    return 0;
}

static const luaL_Reg lua_registers[] =
{
    { "read", lua_register_read },
    { "write", lua_register_write },
    { nullptr, nullptr },
};

bool luaopen_registers(lua_State *L)
{
    luaL_newlib(L, lua_registers);
    lua_setglobal(L, "registers");

    return true;
}
