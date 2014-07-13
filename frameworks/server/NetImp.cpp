#include "NetImp.h"

#include "NetManager.h"
#include "Log.h"
#include "BitStream.h"
#include "VarList.h"

enum VarType
{
	VAR_UNDEFINED,
	VAR_BOOL,
	VAR_INT,
	VAR_FLOAT,
	VAR_STRING,
	VAR_RESERVED_DONT_USE_1, // wide string
	VAR_RESERVED_DONT_USE_2, // object
	VAR_RESERVED_DONT_USE_3, // pointer
	VAR_RESERVED_DONT_USE_4, // userdata

	VAR_INT64,
	VAR_NUMBER,

	VAR_BYTE,
	VAR_SHORT,
};

NetSocket * createNetSocket()
{
	return new NetImp;
}

/*
static int handleUserExtEvent(int handler, cocos2d::ScriptHandlerMgr::HandlerType handlerType, cocos2d::LuaValueArray &args)
{
	cocos2d::LuaStack *stack = cocos2d::LuaEngine::getInstance()->getLuaStack();
	stack->pushInt((int)handlerType);
	stack->pushLuaValueArray(args);
	int ret = stack->executeFunctionByHandler(handler, 2);
	stack->clean();
	return ret;
}

static void sendCustomEvent(void *object, cocos2d::ScriptHandlerMgr::HandlerType handlerType, cocos2d::LuaValueArray &args)
{
#if CC_ENABLE_SCRIPT_BINDING
	int nHandler = cocos2d::ScriptHandlerMgr::getInstance()->getObjectHandler((void*)object, handlerType);
	if (0 != nHandler) 
	{
		handleUserExtEvent(nHandler, handlerType, args);
	}
#endif // #if CC_ENABLE_SCRIPT_BINDING
}

NetSocket *GetNetImp()
{	
	static NetImp obj;

	size_t index = 0;
	if (!NetManager::instance()->find(&obj, index)) 
	{
		NetManager::instance()->add(&obj);
	}

	BitStream::setDefaultByteOrder(BitStream::LITTLE_ENDIAN);

	return (NetSocket *)&obj;
}
*/

void NetImp::onConnected()
{
	if (NetManager::instance()->hasConnectCallback())
	{
		NetManager::instance()->getConnectCallback()(this);
	}
}

void NetImp::onDisconnect()
{
	if (NetManager::instance()->hasDisconnectCallback())
	{
		NetManager::instance()->getDisconnectCallback()(this);
	}
}

int NetImp::onReadLength(unsigned char *buffer, size_t bytes, size_t &offset)
{
	size_t length_size = sizeof(int);
	if (bytes > length_size)
	{
		// read length
		BitStream bs(buffer, bytes);
		int len = bs.readInt32();

		// is whole package
		if (bytes >= length_size + len)
		{
			offset = length_size; 
			return len;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

void NetImp::onRead(BitStream &packet_stream)
{
	//Log("do_read:");
	//LogHex((char *)packet_stream.buffer(), packet_stream.size());

	VarList args;

	unsigned int id = packet_stream.readInt32();	
	while (!packet_stream.isEnd())
	{
		int type = (int)packet_stream.readInt8();
		switch (type)
		{
		case Var::BOOL:
			args.add((packet_stream.readInt8() ? true : false));
			break;
		case Var::BYTE:
			args.add((packet_stream.readInt8()));
			break;
		case Var::SHORT:
			args.add((packet_stream.readInt16()));
			break;
		case Var::INT:
			args.add((packet_stream.readInt32()));
			break;
		case Var::INT64:
			args.add((packet_stream.readInt64()));
			break;
		case Var::FLOAT:
			args.add((packet_stream.readFloat()));
			break;
		case Var::NUMBER:
			args.add((packet_stream.readDouble()));
			break;
		case Var::STRING:
			{
				int length = packet_stream.readInt16();
				assert(length >= 0);
				if (length == 0)
				{
					args.add((""));
				}
				else
				{
					uint8_t *buffer = (uint8_t *)malloc(length+1);
					assert(buffer);
					packet_stream.readData(buffer, length);
					buffer[length] = 0;
					args.add(((const char *)buffer));
					free(buffer);
				}
			}
			break;
		default:
			assert(false);
			break;
		}
	}

	if (NetManager::instance()->hasReadCallback())
	{
		NetManager::instance()->getReadCallback()(this, id, args);
	}
}

void NetImp::writePacket( unsigned int id, const VarList &args )
{
	BitStream bs;

	// packet to bitstream
	bs.writeInt32(id);
	for (int i=0; i<args.count(); ++i)
	{
		int type = args.get(i).getType();
		switch (type)
		{
		case Var::BOOL:
			bs.writeInt8(VAR_BYTE);
			bs.writeInt8(args.get(i).toBool() ? 1 : 0);
			break;
		case Var::BYTE:
			bs.writeInt8(VAR_BYTE);
			bs.writeInt8(args.get(i).toByte());
			break;
		case Var::SHORT:
			bs.writeInt8(VAR_SHORT);
			bs.writeInt16(args.get(i).toShort());
			break;
		case Var::INT:
			bs.writeInt8(VAR_INT);
			bs.writeInt32(args.get(i).toInt());
			break;
		case Var::INT64:
			bs.writeInt8(VAR_INT64);
			bs.writeInt64(args.get(i).toInt64());
			break;
		case Var::FLOAT:
			bs.writeInt8(VAR_FLOAT);
			bs.writeFloat(args.get(i).toFloat());
			break;
		case Var::NUMBER:
			bs.writeInt8(VAR_NUMBER);
			bs.writeDouble(args.get(i).toNumber());
			break;
		case Var::STRING:
			{
				std::string s = args.get(i).toString();
				bs.writeInt8(VAR_STRING);
				bs.writeInt16(s.length());
				bs.writeData((uint8_t *)s.c_str(), s.length());
			}
			break;
		default:
			assert(false);
			break;
		}	
	}

	// write length
	BitStream bs_length;
	bs_length.writeInt32(bs.size());
	this->write(bs_length.buffer(), bs_length.size());

	// write data
	this->write(bs.buffer(), bs.size());

	//Log("do_write:");
	//LogHex((char *)bs_length.buffer(), bs_length.size());
	//LogHex((char *)bs.buffer(), bs.size());
}
