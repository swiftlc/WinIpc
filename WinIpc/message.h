#pragma once

#include <string>
#include <queue>

#include "common.h"

namespace WinIpc {

  class Message;

  class MessageReader
  {
  public:
    MessageReader() : read_ptr_(NULL), read_end_ptr_(NULL) {}
    explicit MessageReader(Message* m);

    bool ReadBool(bool* result);
    bool ReadInt(int* result);
    bool ReadUInt16(uint16* result);
    bool ReadUInt32(uint32* result);
    bool ReadInt64(int64* result);
    bool ReadUInt64(uint64* result);
    bool ReadFloat(float* result);
    bool ReadString(std::string* result);
    bool ReadWString(std::wstring* result);
    bool ReadData(const char** data, int* length);
    bool ReadBytes(const char** data, int length);

  private:
    template <typename Type>
    inline bool ReadBuiltinType(Type* result);

    const char* GetReadPointerAndAdvance(int num_bytes);

    inline const char* GetReadPointerAndAdvance(int num_elements,
      size_t size_element);

    const char* read_ptr_;
    const char* read_end_ptr_;
  };

  class Message {
  public:
    Message();

    Message(int32 routing_id, uint32 type);

    Message(const char* data, int data_len);

    ~Message();

    // Returns the size of the Pickle's data.
    size_t size() const { return kHeaderSize + header_->payload_size; }

    // Returns the data for this Pickle.
    const void* data() const { return header_; }

    // The payload is the pickle data immediately following the header.
    size_t payload_size() const { return header_->payload_size; }

    const char* payload() const {
      return reinterpret_cast<const char*>(header_) + kHeaderSize;
    }

    // Returns the address of the byte immediately following the currently valid
    // header + payload.
    const char* end_of_payload() const {
      // This object may be invalid.
      return header_ ? payload() + payload_size() : NULL;
    }

    uint32 type() const {
      return header()->type;
    }

    int32 routing_id() const {
      return header()->routing;
    }

    void set_routing_id(int32 new_id) {
      header()->routing = new_id;
    }

    // Sets all the given header values. The message should be empty at this
    // call.
    void SetHeaderValues(int32 routing, uint32 type, uint32 flags);

    bool WriteBool(bool value) {
      return WriteInt(value ? 1 : 0);
    }

    bool WriteInt(int value) {
      return WriteBytes(&value, sizeof(value));
    }

    bool WriteUInt16(uint16 value) {
      return WriteBytes(&value, sizeof(value));
    }

    bool WriteUInt32(uint32 value) {
      return WriteBytes(&value, sizeof(value));
    }

    bool WriteInt64(int64 value) {
      return WriteBytes(&value, sizeof(value));
    }

    bool WriteUInt64(uint64 value) {
      return WriteBytes(&value, sizeof(value));
    }

    bool WriteFloat(float value) {
      return WriteBytes(&value, sizeof(value));
    }

    bool WriteString(const std::string& value);

    bool WriteString(const std::wstring& value);

    // "Data" is a blob with a length. When you read it out you will be given the
    // length. See also WriteBytes.
    bool WriteData(const char* data, int length);

    // "Bytes" is a blob with no length. The caller must specify the lenght both
    // when reading and writing. It is normally used to serialize PoD types of a
    // known size. See also WriteData.
    bool WriteBytes(const void* data, int data_len);

    // 从buffer中寻找下一条消息的起始地址
    static const char* FindNext(const char* range_start, const char* range_end);
  protected:
    friend class Channel;

#pragma pack(push, 4)
    struct Header {
      int32 routing;  // ID of the view that this message is destined for
      uint32 type;    // specifies the user-defined message type
      uint32 flags;   // specifies control flags for the message
      uint32 payload_size;
    };//16 BYTES
#pragma pack(pop)

    Header* header();
    const Header* header() const;

    // Resize the capacity, note that the input value should include the size of
    // the header: new_capacity = sizeof(Header) + desired_payload_capacity.
    // A realloc() failure will cause a Resize failure... and caller should check
    // the return result for true (i.e., successful resizing).
    bool Resize(size_t new_capacity);

    // Aligns 'i' by rounding it up to the next multiple of 'alignment'
    static size_t AlignInt(size_t i, int alignment) {
      return i + (alignment - (i % alignment)) % alignment;
    }

    static const uint32 kHeaderSize;

    Header* header_;
    // Allocation size of payload (or -1 if allocation is const).
    size_t capacity_;
  };

  using MessagePtr = std::shared_ptr<Message>;

  using MessageQueue = std::queue<MessagePtr>;

  template<typename ...Args>
  inline MessagePtr MakeMessage(Args&&...args) { return std::make_shared<Message>(std::forward<Args>(args)...); }
}