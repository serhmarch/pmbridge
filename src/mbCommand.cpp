#include "mbCommand.h"

#include "mbMemory.h"
#include "mbClient.h"

mbCommand::~mbCommand()
{
}


/************************************************************************
 ********************************* QUERY ********************************
 ************************************************************************/

 mbCommandQuery::mbCommandQuery(mbMemory *memory, mbClient *client) :
    m_memory(memory),
    m_client(client),
    m_unit(0),
    m_exec(0),
    m_succAdr(),
    m_errcAdr(),
    m_errvAdr(),
    m_isBegin(true)
{
    m_buffer.resize(MB_MAX_BYTES);
}

mbCommandQuery::~mbCommandQuery()
{
}

bool mbCommandQuery::run()
{
    if (m_isBegin)
    {
        Modbus::StatusCode status = beginQuery();
        if (Modbus::StatusIsBad(status))
        {
            m_memory->setUInt16(m_errcAdr, m_memory->getUInt16(m_succAdr) + 1);
            m_memory->setUInt16(m_errvAdr, static_cast<uint16_t>(status));
            return true;
        }
        m_isBegin = false;
    }
    Modbus::StatusCode status = runQuery();
    if (Modbus::StatusIsProcessing(status))
        return false;
    if (Modbus::StatusIsGood(status))
    {
        m_memory->setUInt16(m_succAdr, m_memory->getUInt16(m_succAdr) + 1);
    }
    else
    {
        m_memory->setUInt16(m_errcAdr, m_memory->getUInt16(m_succAdr) + 1);
        m_memory->setUInt16(m_errvAdr, static_cast<uint16_t>(status));
    }
    m_isBegin = true;
    return true;
}

Modbus::StatusCode mbCommandQuery::beginQuery()
{
    return Modbus::Status_Good;
}

Modbus::StatusCode mbCommandQueryReadCoils::runQuery()
{
    Modbus::StatusCode status = m_client->readCoils(m_unit, m_offset, m_count, m_buffer.data());
    if (Modbus::StatusIsGood(status))
    {
        m_memory->write_bits(m_memAdr, m_count, m_buffer.data());
    }
    return status;
}

Modbus::StatusCode mbCommandQueryReadDiscreteInputs::runQuery()
{
    Modbus::StatusCode status = m_client->readDiscreteInputs(m_unit, m_offset, m_count, m_buffer.data());
    if (Modbus::StatusIsGood(status))
    {
        m_memory->write_bits(m_memAdr, m_count, m_buffer.data());
    }
    return status;
}

Modbus::StatusCode mbCommandQueryReadHoldingRegisters::runQuery()
{
    Modbus::StatusCode status = m_client->readHoldingRegisters(m_unit, m_offset, m_count, reinterpret_cast<uint16_t*>(m_buffer.data()));
    if (Modbus::StatusIsGood(status))
    {
        m_memory->write_bits(m_memAdr, m_count, m_buffer.data());
    }
    return status;
}

Modbus::StatusCode mbCommandQueryReadInputRegisters::runQuery()
{
    Modbus::StatusCode status = m_client->readHoldingRegisters(m_unit, m_offset, m_count, reinterpret_cast<uint16_t*>(m_buffer.data()));
    if (Modbus::StatusIsGood(status))
    {
        m_memory->write_bits(m_memAdr, m_count, m_buffer.data());
    }
    return status;
}

Modbus::StatusCode mbCommandQueryWriteMultipleCoils::beginQuery()
{
    return m_memory->read_bits(m_memAdr, m_count, m_buffer.data());
}

Modbus::StatusCode mbCommandQueryWriteMultipleCoils::runQuery()
{
    return m_client->writeMultipleCoils(m_unit, m_offset, m_count, m_buffer.data());;
}

Modbus::StatusCode mbCommandQueryWriteMultipleRegisters::beginQuery()
{
    return m_memory->read_regs(m_memAdr, m_count, m_buffer.data());
}

Modbus::StatusCode mbCommandQueryWriteMultipleRegisters::runQuery()
{
    return m_client->writeMultipleCoils(m_unit, m_offset, m_count, m_buffer.data());;
}


/************************************************************************
 ********************************* COPY *********************************
 ************************************************************************/

 mbCommandCopy::mbCommandCopy(mbMemory *memory) :
    m_memory(memory),
    m_count(0)
{
}

bool mbCommandCopy::run()
{
    m_memory->copy(m_srcAdr, m_dstAdr, m_count);
    return true;
}


/************************************************************************
 ********************************* DUMP *********************************
 ************************************************************************/

 mbCommandDump::mbCommandDump(mbMemory *memory) :
    m_memory(memory),
    m_format(mb::Format_Hex16),
    m_count(0)
{
}

bool mbCommandDump::run()
{
    m_memory->dump(m_memAdr, m_format, m_count);
    return true;
}


/************************************************************************
 ********************************* DELAY ********************************
 ************************************************************************/

 mbCommandDelay::mbCommandDelay() :
    m_timer(0),
    m_millis(0),
    m_isBegin(true)
{
}

bool mbCommandDelay::run()
{
    if (m_isBegin)
    {
        m_timer = Modbus::timer();
        m_isBegin = false;
    }
    Modbus::msleep(1);
    if (Modbus::timer() - m_timer >= m_millis)
    {
        m_isBegin = true;
        return true;
    }
    return false;
}

/*
#include <stdio.h>
#include <stdlib.h>
#include "bridged.h"
#include "memory.h"
#include "modbus.h"

void cmd_query_exec(cmd_t* cmd)
{
    const int max_ds = 2000;
    const int max_rs = 100;
    int cmd_en, mbr;
    unsigned short sc, fc, fact;
    mbdev_t* dev;
    cmdquery_t* query;
    char d_buff[max_ds];
    unsigned short r_buff[max_rs];

    query = cmd->c.query;
    dev = query->dev;
    cmd_en = (query->ic == 0) || (query->ic == 1) || (query->cycle % query->ic == 0);
    query->cycle++;
    if (cmd_en)
    {
        memory_read3x(query->sc_adr, 1, &sc);
        memory_read3x(query->fc_adr, 1, &fc);
        
        if (!mb_is_connected(dev))
        {
            mbr = mb_connect(dev);
            if (mbr != MBR_OK)
            {
                fc++;
                memory_write3x(query->fc_adr, 1, &fc);
                memory_write3x(query->err_adr, 1, (unsigned short*)&mbr);
                return;
            }
        }
        mbr = MBR_OK;
        switch (query->func)
        {
        case MBF_READ_COIL_STATUS:
            mbr = mb_read_coil_status(dev, query->slave, query->from_adr, query->count, d_buff, &fact);
            break;
        case MBF_READ_INPUT_STATUS:
            mbr = mb_read_input_status(dev, query->slave, query->from_adr, query->count, d_buff, &fact);
            break;
        case MBF_READ_HOLDING_REGISTERS:
            mbr = mb_read_holding_registers(dev, query->slave, query->from_adr, query->count, r_buff, &fact);
            break;
        case MBF_READ_INPUT_REGISTERS:
            mbr = mb_read_input_registers(dev, query->slave, query->from_adr, query->count, r_buff, &fact);
            break;
        case MBF_FORCE_MULTIPLE_COILS:
            fact = (unsigned short)memory_read0x(query->from_adr, query->count, d_buff);
            break;
        case MBF_FORCE_MULTIPLE_REGISTERS:
            fact = (unsigned short)memory_read4x(query->from_adr, query->count, r_buff);
            break;
        }        
        if (mbr != MBR_OK)
        {
            fc++;
            memory_write3x(query->fc_adr, 1, &fc);
            memory_write3x(query->err_adr, 1, (unsigned short*)&mbr);
            return;
        }

        switch (query->func)
        {
        case MBF_READ_COIL_STATUS:
            memory_write0x(query->to_adr, fact, d_buff);
            break;
        case MBF_READ_INPUT_STATUS:
            memory_write1x(query->to_adr, fact, d_buff);
            break;
        case MBF_READ_HOLDING_REGISTERS:
            memory_write4x(query->to_adr, fact, r_buff);
            break;
        case MBF_READ_INPUT_REGISTERS:
            memory_write3x(query->to_adr, fact, r_buff);
            break;
        case MBF_FORCE_MULTIPLE_COILS:
            mbr = mb_force_multiple_coils(dev, query->slave, query->to_adr, fact, d_buff, NULL);
            break;
        case MBF_FORCE_MULTIPLE_REGISTERS:
            mbr = mb_force_multiple_registers(dev, query->slave, query->to_adr, fact, r_buff, NULL);
            break;
        }
        if (mbr != MBR_OK)
        {
            fc++;
            memory_write3x(query->fc_adr, 1, &fc);
            memory_write3x(query->err_adr, 1, (unsigned short*)&mbr);
            return;
        }
        sc++;
        memory_write3x(query->sc_adr, 1, &sc);
    }
}

void cmd_copy_exec(cmd_t* cmd)
{
    cmdcopy_t* copy = cmd->c.copy;
    memory_copy(copy->from_type, copy->from_adr, copy->count, copy->to_type, copy->to_adr);
}

void cmd_delay_exec(cmd_t* cmd)
{
    cmddelay_t* delay = cmd->c.delay;
    usleep(delay->msec*1000);
}

void cmd_dump_exec(cmd_t* cmd)
{
    const int max_sz = 0xFFFF;
    int i, j, c, n;
    unsigned short r_buff[max_sz];
    char d_buff[max_sz], t_buff[max_sz];
    cmddump_t* dump = cmd->c.dump;
    switch (dump->type)
    {
    case MEMORY_0X:
        c = memory_read0x(dump->adr, dump->count, d_buff);
        break;
    case MEMORY_1X:
        c = memory_read1x(dump->adr, dump->count, d_buff);
        break;
    case MEMORY_3X:
        c = memory_read3x(dump->adr, dump->count, r_buff);
        break;
    case MEMORY_4X:
        c = memory_read4x(dump->adr, dump->count, r_buff);
        break;
    default:
        return;
    }
    t_buff[0] = 0;
    for (i = 0, j = 0; i < c; i++)
    {
        if (dump->type == MEMORY_0X || dump->type == MEMORY_1X)
            n = sprintf(&t_buff[j], "%hhu ", d_buff[i]);
        else if (dump->numsys == 8)
            n = sprintf(&t_buff[j], "%06ho ", r_buff[i]);
        else if (dump->numsys == 16)
            n = sprintf(&t_buff[j], "%04hX ", r_buff[i]);
        else
            n = sprintf(&t_buff[j], "%hu ", r_buff[i]);
        j += n;
    }
    bridged_info(t_buff);
}

void cmd_exec(cmd_t* cmd)
{
    switch (cmd->type)
    {
    case CMD_QUERY: cmd_query_exec(cmd); break;
    case CMD_COPY:  cmd_copy_exec(cmd);  break;
    case CMD_DELAY: cmd_delay_exec(cmd); break;
    case CMD_DUMP:  cmd_dump_exec(cmd);  break;
    }
}

cmd_t* new_cmd_query(mbdev_t* dev, unsigned char slave, unsigned char func, unsigned short from_adr, unsigned short count, 
                     unsigned short to_adr,unsigned short ic, unsigned short  sc_adr, unsigned short  fc_adr, unsigned short  err_adr)
{
    cmdquery_t* query = (cmdquery_t*)malloc(sizeof(cmdquery_t));
    cmd_t* cmd = (cmd_t*)malloc(sizeof(cmd_t));

    query->dev      = dev;
    query->slave    = slave;
    query->func     = func;
    query->from_adr = from_adr;
    query->count    = count;
    query->to_adr   = to_adr;
    query->ic       = ic;
    query->sc_adr   = sc_adr;
    query->fc_adr   = fc_adr;
    query->err_adr  = err_adr;
    query->cycle    = 0;
    cmd->type = CMD_QUERY;
    cmd->c.query = query;
    return cmd;
}

cmd_t* new_cmd_copy(char from_type, unsigned short from_adr, unsigned short count, char to_type, unsigned short to_adr)
{
    cmdcopy_t* copy = (cmdcopy_t*)malloc(sizeof(cmdcopy_t));
    cmd_t* cmd = (cmd_t*)malloc(sizeof(cmd_t));

    copy->from_type = from_type;
    copy->from_adr  = from_adr;
    copy->count     = count;
    copy->to_type   = to_type;
    copy->to_adr    = to_adr;
    cmd->type = CMD_COPY;
    cmd->c.copy = copy;
    return cmd;
}

cmd_t* new_cmd_delay(unsigned long msec)
{
    cmddelay_t* delay = (cmddelay_t*)malloc(sizeof(cmddelay_t));
    cmd_t* cmd = (cmd_t*)malloc(sizeof(cmd_t));

    delay->msec = msec;
    cmd->type = CMD_DELAY;
    cmd->c.delay = delay;
    return cmd;
}

cmd_t* new_cmd_dump(char type, unsigned short adr, unsigned short count, char numsys)
{
    cmddump_t* dump = (cmddump_t*)malloc(sizeof(cmddump_t));
    cmd_t* cmd = (cmd_t*)malloc(sizeof(cmd_t));

    dump->type  = type;
    dump->adr   = adr;
    dump->count = count;
    dump->numsys = numsys;
    cmd->type = CMD_DUMP;
    cmd->c.dump = dump;
    return cmd;
}

void delete_cmd(cmd_t* cmd)
{
    switch (cmd->type)
    {
    case CMD_QUERY:
        free(cmd->c.query);
        break;
    case CMD_COPY:
        free(cmd->c.copy);
        break;
    case CMD_DELAY:
        free(cmd->c.delay);
        break;
    case CMD_DUMP:
        free(cmd->c.dump);
        break;
    }
    free(cmd);
}

const char* str_cmd_type(char type)
{
    switch (type)
    {
    case CMD_QUERY:
        return "QUERY";
    case CMD_COPY:
        return "COPY";
    case CMD_DELAY:
        return "DELAY";
    case CMD_DUMP:
        return "DUMP";
    }
    return "UNKNOWN";
}
*/

