#pragma once
#include <sstream>
#include "Clip.h"
#include "ISerializable.h"
class ClipsCollection : public ISerializable
{
public:
	ClipsCollection();
	~ClipsCollection();

	unsigned __int16						Version() noexcept override;
	std::string								Serialize() override;
	void									Deserialize(std::string serializationData) override;

	void									AddFront(std::shared_ptr<Clip> clip);
	std::shared_ptr<Clip>					RemoveBack();
	std::shared_ptr<Clip>					RemoveAt(size_t index);
	std::shared_ptr<Clip>					At(size_t index);
	std::shared_ptr<Clip>					Front();
	unsigned int							Size();
	unsigned int							MaxSize();
	void									SetMaxSize(unsigned int maxSize);
	void									Clear();

private:
	std::deque<std::shared_ptr<Clip>>		clips;
	unsigned int							maxSize = 200;
};

