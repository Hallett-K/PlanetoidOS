#include "exception.hpp"
#include "core/interrupts.hpp"
#include "core/log.hpp"
#include "gic.hpp"
#include "timer.hpp"

namespace SyncExceptionErrors
{
    const uint64_t ERR_UNKNOWN = 0x00;
    const uint64_t ERR_SVC = 0x15;
    const uint64_t ERR_ABORT_INS_LOWER = 0x20;
    const uint64_t ERR_ABORT_INS_CURRENT = 0x21;
    const uint64_t ERR_ABORT_DATA_LOWER = 0x24;
    const uint64_t ERR_ABORT_DATA_CURRENT = 0x25;
};

namespace DataAbortFaults
{
    const uint64_t FLT_ADDRESS_SIZE_L0 = 0x00;
    const uint64_t FLT_ADDRESS_SIZE_L1 = 0x01;
    const uint64_t FLT_ADDRESS_SIZE_L2 = 0x02;
    const uint64_t FLT_ADDRESS_SIZE_L3 = 0x03;

    const uint64_t FLT_TRANSLATION_L0 = 0x04;
    const uint64_t FLT_TRANSLATION_L1 = 0x05;
    const uint64_t FLT_TRANSLATION_L2 = 0x06;
    const uint64_t FLT_TRANSLATION_L3 = 0x07;

    const uint64_t FLT_ACCESS_L1 = 0x09;
    const uint64_t FLT_ACCESS_L2 = 0x0A;
    const uint64_t FLT_ACCESS_L3 = 0x0B;

    const uint64_t FLT_PERMISSION_L1 = 0x0D;
    const uint64_t FLT_PERMISSION_L2 = 0x0E;
    const uint64_t FLT_PERMISSION_L3 = 0x0F;

    const uint64_t FLT_SYNC_EXTERNAL_ABORT = 0x10;
    const uint64_t FLT_SYNC_EXTERNAL_ABORT_TTW_L0 = 0x14;
    const uint64_t FLT_SYNC_EXTERNAL_ABORT_TTW_L1 = 0x15;
    const uint64_t FLT_SYNC_EXTERNAL_ABORT_TTW_L2 = 0x16;
    const uint64_t FLT_SYNC_EXTERNAL_ABORT_TTW_L3 = 0x17;

    const uint64_t FLT_SYNC_PARITY = 0x18;
    const uint64_t FLT_SYNC_PARITY_TTW_L0 = 0x1C;
    const uint64_t FLT_SYNC_PARITY_TTW_L1 = 0x1D;
    const uint64_t FLT_SYNC_PARITY_TTW_L2 = 0x1E;
    const uint64_t FLT_SYNC_PARITY_TTW_L3 = 0x1F;

    const uint64_t FLT_ALIGNMENT = 0x21;
    const uint64_t FLT_DEBUG = 0x22;
    const uint64_t FLT_TLB_CONFLICT = 0x30;
    const uint64_t FLT_ATOMIC = 0x31;
};

uint64_t get_exception_class(uint64_t esr)
{
    return (esr >> 26) & 0x3F;
}

const char* decode_abort_error(uint64_t esr)
{
    const uint64_t fsc = esr & 0x3F;

    switch (fsc)
    {
        case DataAbortFaults::FLT_ADDRESS_SIZE_L0:
            return "Address size fault at level 0";
        case DataAbortFaults::FLT_ADDRESS_SIZE_L1:
            return "Address size fault at level 1";
        case DataAbortFaults::FLT_ADDRESS_SIZE_L2:
            return "Address size fault at level 2";
        case DataAbortFaults::FLT_ADDRESS_SIZE_L3:
            return "Address size fault at level 3";

        case DataAbortFaults::FLT_TRANSLATION_L0:
            return "Translation fault at level 0";
        case DataAbortFaults::FLT_TRANSLATION_L1:
            return "Translation fault at level 1";
        case DataAbortFaults::FLT_TRANSLATION_L2:
            return "Translation fault at level 2";
        case DataAbortFaults::FLT_TRANSLATION_L3:
            return "Translation fault at level 3";

        case DataAbortFaults::FLT_ACCESS_L1:
            return "Access flag fault at level 1";
        case DataAbortFaults::FLT_ACCESS_L2:
            return "Access flag fault at level 2";
        case DataAbortFaults::FLT_ACCESS_L3:
            return "Access flag fault at level 3";

        case DataAbortFaults::FLT_PERMISSION_L1:
            return "Permission fault at level 1";
        case DataAbortFaults::FLT_PERMISSION_L2:
            return "Permission fault at level 2";
        case DataAbortFaults::FLT_PERMISSION_L3:
            return "Permission fault at level 3";

        case DataAbortFaults::FLT_SYNC_EXTERNAL_ABORT:
            return "Sync external abort";
        case DataAbortFaults::FLT_SYNC_EXTERNAL_ABORT_TTW_L0:
            return "Sync external abort during translation table walk at level 0";
        case DataAbortFaults::FLT_SYNC_EXTERNAL_ABORT_TTW_L1:
            return "Sync external abort during translation table walk at level 1";
        case DataAbortFaults::FLT_SYNC_EXTERNAL_ABORT_TTW_L2:
            return "Sync external abort during translation table walk at level 2";
        case DataAbortFaults::FLT_SYNC_EXTERNAL_ABORT_TTW_L3:
            return "Sync external abort during translation table walk at level 3";

        case DataAbortFaults::FLT_SYNC_PARITY:
            return "Sync parity fault";
        case DataAbortFaults::FLT_SYNC_PARITY_TTW_L0:
            return "Sync parity fault during translation table walk at level 0";
        case DataAbortFaults::FLT_SYNC_PARITY_TTW_L1:
            return "Sync parity fault during translation table walk at level 1";
        case DataAbortFaults::FLT_SYNC_PARITY_TTW_L2:
            return "Sync parity fault during translation table walk at level 2";
        case DataAbortFaults::FLT_SYNC_PARITY_TTW_L3:
            return "Sync parity fault during translation table walk at level 3";

        case DataAbortFaults::FLT_ALIGNMENT:
            return "Alignment fault";
        case DataAbortFaults::FLT_DEBUG:
            return "Debug fault";
        case DataAbortFaults::FLT_TLB_CONFLICT:
            return "TLB Conflict fault";
        case DataAbortFaults::FLT_ATOMIC:
            return "Atomic fault";

        default:
            return "Unknown data abort fault";
    }
}

extern "C" void sync_exception_handler(exception_context* context)
{
    Log::Error("Sync exception received!");

    const uint64_t exception_class = get_exception_class(context->esr);
    const uint8_t write = (context->esr & (1ULL << 6)) != 0;
    switch (exception_class)
    {
        case SyncExceptionErrors::ERR_UNKNOWN:
            Log::Error("Unknown or undefined instruction");
            break;
        case SyncExceptionErrors::ERR_SVC:
            Log::Error("SVC Instruction");
            break;
        case SyncExceptionErrors::ERR_ABORT_INS_LOWER:
            Log::Error("Instruction abort from lower exception level");
            Log::Error(decode_abort_error(context->esr));
            break;
        case SyncExceptionErrors::ERR_ABORT_INS_CURRENT:
            Log::Error("Instruction abort from current exception level");
            Log::Error(decode_abort_error(context->esr));
            break;
        case SyncExceptionErrors::ERR_ABORT_DATA_LOWER:
            Log::Error("Data abort from lower exception level");
            Log::Error(decode_abort_error(context->esr));
            if (write)
                Log::Error("Fault was a write operation");
            else
                Log::Error("Fault was a read operation");
            break;
        case SyncExceptionErrors::ERR_ABORT_DATA_CURRENT:
            Log::Error("Data abort from current exception level");
            Log::Error(decode_abort_error(context->esr));
            if (write)
                Log::Error("Fault was a write operation");
            else
                Log::Error("Fault was a read operation");
            break;
        default:
            Log::Error("Unknown Error");
            break;
    }

    while (true)
    {
        asm volatile("wfe");
    }
}

extern "C" void irq_exception_handler(exception_context* context)
{
    (void)context;

    const uint32_t interrupt_id = GIC::acknowledge_interrupt();
    
    Interrupts::dispatch_interrupt(interrupt_id, context);

    GIC::end_interrupt(interrupt_id);
}

extern "C" void fiq_exception_handler()
{
    Log::Error("FIQ Exception");

    while (true)
    {
        asm volatile("wfe");
    }
}

extern "C" void serror_exception_handler()
{
    Log::Error("SError Exception");

    while (true)
    {
        asm volatile("wfe");
    }
}