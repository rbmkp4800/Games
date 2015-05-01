#include <extypes.h>

namespace BallsGame
{
	enum class Control : uint32
	{
		None = 0,
		Up = 1,
		Down = 2,
		Left = 3,
		Right = 4,
		PositiveCharge = 5,
		NegativeCharge = 6,
		Slow = 7,
	};

	class Game abstract
	{
	private:
		static const uint32 messagesLimit = 256;

		enum class MessageType : uint32
		{
			None = 0,
			ChangeControlState = 1,
			ResizeOutput = 2,
			Restart = 3,
		};
		struct Message
		{
			MessageType type;
			uint32 a, b;
			inline Message() {}
			inline Message(MessageType _type, uint32 _a = 0, uint32 _b = 0) : type(_type), a(_a), b(_b) {}
		};

		static Message messages[messagesLimit];
		static uint32 firstMessage, lastMessage;
		static bool initialized;

		static void InitMessagesStack() { initialized = true; firstMessage = 0; lastMessage = 0; }
		static void PushMessage(Message message)
		{
			uint32 newLastMessage = (lastMessage + 1) % messagesLimit;
			if (initialized && newLastMessage != firstMessage)
			{
				messages[lastMessage] = message;
				lastMessage = newLastMessage;
			}
		}
		static Message PopMessage()
		{
			if (initialized && firstMessage != lastMessage)
			{
				uint32 oldFirstMessage = firstMessage;
				firstMessage = (firstMessage + 1) % messagesLimit;
				return messages[oldFirstMessage];
			}
			return Message(MessageType::None, 0, 0);
		}

	public:
		static void Run(void* output, uint32 outputSizeX, uint32 outputSizeY);

		static inline void SetControlState(Control control, bool state)
		{
			Game::PushMessage(Message(MessageType::ChangeControlState, (uint32) control, (uint32) state));
		}
		static inline void ResizeOutput(uint32 x, uint32 y)
		{
			Game::PushMessage(Message(MessageType::ResizeOutput, x, y));
		}
		static inline void Restart()
		{
			Game::PushMessage(Message(MessageType::Restart));
		}
	};
}