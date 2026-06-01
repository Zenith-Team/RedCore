#pragma once

#include <basis/seadTypes.h>

namespace red {
    
    class StrybbleUtil {
    private:
        static constexpr const char cCharMap[64] = {
            '\0', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
            'h',  'i', 'j', 'k', 'l', 'm', 'n', 'o',
            'p',  'q', 'r', 's', 't', 'u', 'v', 'w',
            'x',  'y', 'z', '0', '1', '2', '3', '4',
            '5',  'A', 'B', 'C', 'D', 'E', 'F', 'G',
            'H',  'I', 'J', 'K', 'L', 'M', 'N', 'O',
            'P',  'Q', 'R', 'S', 'T', 'U', 'V', 'W',
            'X',  'Y', 'Z', '6', '7', '8', '9', '_'
        };
        
    public:
        /**
         * @brief Decodes an array of Strybble charcodes into a proper ASCII string.
         * @param encoded The input array of Strybble charcodes.
         * @param decoded The output array to write the decoded string into.
         * @tparam TInCount Deduced size of the input array.
         * @tparam TOutCount Deduced size of the output array.
         * @return The number of characters written to the output buffer, excluding the null terminator.
         */
        template <s32 TInCount, s32 TOutCount>
        static s32 decodeFromChars(const u8 (&encoded)[TInCount], u8 (&decoded)[TOutCount]) {
            static_assert(TOutCount >= TInCount, "Decoded buffer must be larger than encoded buffer.");

            s32 processCount = TInCount;
            if (encoded[TInCount - 1] == '\0') {
                processCount--;
            }

            char buffer[TInCount + 3] = { 0 }; // safe lookahead for underscore chain sequences
            
            for (s32 i = 0; i < processCount; i++) {
                buffer[i] = cCharMap[static_cast<u8>(encoded[i])];
            }
            buffer[processCount] = '\0';
            
            s32 newCharCount = 0;
            for (s32 i = 0; i < processCount; i++) {
                if (buffer[i] == '\0')
                    continue;
                
                if (buffer[i] == '_' && buffer[i + 1] == '_') {
                    if (buffer[i + 2] == '_') {
                        decoded[newCharCount++] = '.';
                        i += 2;
                    } else {
                        decoded[newCharCount++] = '/';
                        i += 1;
                    }
                } else {
                    decoded[newCharCount++] = buffer[i];
                }
            }
            
            decoded[newCharCount] = '\0';
            return newCharCount;
        }
        
        /**
         * @brief Unpacks a packed 6-bit-per-code bitstream into an array of Strybble character codes.
         * @param bitstream The input array of bytes containing the bitstream to unpack.
         * @param decoded The output buffer to write the decoded codes into.
         * @param charCount The number of characters to decode. @p decoded must be sized charCount + 1.
         * @return The number of characters written to the output buffer, excluding the null terminator.
         */
        static s32 decodeFromBitstream(const u8* bitstream, u8* decoded, s32 charCount) {
            for (s32 i = 0; i < charCount; i++) {
                const s32 startBit = i * 6;
                const s32 byte0    = startBit / 8;
                const s32 offset   = startBit % 8;
 
                if (offset <= 2) {
                    decoded[i] = (bitstream[byte0] >> (2 - offset)) & 0x3F;
                } else {
                    const s32 bitsFromByte0 = 8 - offset;
                    decoded[i] = ((bitstream[byte0] & ((1 << bitsFromByte0) - 1)) << (offset - 2)) | (bitstream[byte0 + 1] >> (10 - offset));
                }
            }
 
            decoded[charCount] = '\0';
            return charCount;
        }
    };

} // namespace red
