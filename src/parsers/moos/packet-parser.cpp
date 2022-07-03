#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#include "packet-parser.hpp"

namespace parsers {
namespace moos{


/// # ====== MOOS Network Packet Format ======
/// 
/// ## Per-Packet Header
/// Offset (Bytes)  | Size (Bytes) |    Field
/// ---------------:|-------------:|------------------
///              0  |           4  |    Byte Count
///              4  |           4  |    Message Count
///              8  |           1  |    Compressed flag
/// ----------------|-------------:|-------------------
///                             9  |    Total
///
constexpr static size_t packet_header_length = 9;
struct PacketHeader {
    uint32_t byte_length;
    uint32_t message_count;
    uint8_t compress_flag;
    // stealth pack-bytes are ignored -- all at the end of the struct
};

// // trim spaces from the string_view
// static inline std::string_view trim(std::string_view v) {
//     v.remove_prefix(std::min(v.find_first_not_of(" "), v.size()));
//     v.remove_suffix(v.size() - 1 - v.find_last_not_of(" "));
//     return v;
// }

bool PacketParser::empty() const {
    return (   (nullptr==buffer)
            || (nullptr==cursor)
            || (length <= static_cast<size_t>(cursor - buffer))
    );
}

std::string PacketParser::extract_string(){
    const uint32_t snKey = *reinterpret_cast<const int32_t*>(cursor);
    const char* data = reinterpret_cast<char*>(cursor + 4);
    cursor += 4 + snKey;
    return std::string( data, snKey );
}

bool PacketParser::load( const core::ForwardBuffer* source ){
    /// ## Source code to extract packet header:
    ///    - [simple]    https://github.com/moos-ivp/svn-mirror/blob/master/MOOS_Dec3120/MOOSCore/Core/libMOOS/Comms/MOOSCommPkt.cpp#L244
    ///    - [permalink] https://github.com/moos-ivp/svn-mirror/blob/6d630be212b26a467bd1d935c1a58feae57e044f/MOOS_Dec3120/MOOSCore/Core/libMOOS/Comms/MOOSCommPkt.cpp#L208

    timestamp = source->timestamp;
    length = source->length;
    buffer = source->buffer;
    cursor = source->buffer + packet_header_length;

    const auto* packet_header = reinterpret_cast<const struct PacketHeader *>(buffer);

    // vvvv DEBUG vvvv
    spdlog::trace("        >>> Loading MOOS Packet;    {} message(s);    across ({} =?= {}) bytes.",
                    packet_header->message_count, packet_header->byte_length, length );
    
    // std::cerr << "        ::Byte-Length: " << packet_header->byte_length << std::endl;
    // std::cerr << "        ::Message-Count: " << packet_header->message_count << std::endl;
    // std::cerr << "        ::Compressed:?: " << std::boolalpha << static_cast<bool>(packet_header->compress_flag) << std::endl;
    // std::cerr << "        ::Buffer:@: " << reinterpret_cast<void*>(buffer) << std::endl;
    // std::cerr << "        ::Cursor:@: " << reinterpret_cast<void*>(cursor) << std::endl;
    // fprintf( stderr, "        ::@Cursor:: %02x %02x %02x %02x\n", cursor[0], cursor[1], cursor[2], cursor[3] );
    // ^^^^ DEBUG ^^^^

    if( packet_header->byte_length != length ){
        spdlog::error("        !! mismatch in packet lengths: {} =?= {}) bytes! -- discarding extra bytes.", packet_header->byte_length, length );
        // assert( (packet_header->byte_length == length) && " both lengths should match!?");
        length = packet_header->byte_length;
    }

    return true;
}

core::StringBuffer * PacketParser::next(){
    /// [1] https://github.com/moos-ivp/svn-mirror/blob/master/MOOS_Dec3120/MOOSCore/Core/libMOOS/Comms/MOOSCommObject.cpp#L348
    /// [2] https://github.com/moos-ivp/svn-mirror/blob/master/MOOS_Dec3120/MOOSCore/Core/libMOOS/Comms/MOOSCommPkt.cpp#L244
    /// [3] Extract Bytes of each message:
    ///     https://github.com/moos-ivp/svn-mirror/blob/6d630be212b26a467bd1d935c1a58feae57e044f/MOOS_Dec3120/MOOSCore/Core/libMOOS/Comms/MOOSMsg.cpp#L389
    if( buffer && cursor ){
        uint8_t * message_start = cursor;
        // fprintf( stderr, "        >>> Extract Message: @:index:[ %lu ]\n", (message_start - buffer) );

        /// ## Per-Message Header
        /// Offset (Bytes)  | Size (Bytes) |  Name                   | What
        /// ---------------:|-------------:|------------------------:|---------------------
        ///                 |           4  | m_nLength               | byte count of this message
        ///                 |        *     | m_nID                   | what is message ID;
        ///                 |           1  | m_cMsgType              | what type of message is this?
        ///                 |           1  | m_cDataType             | what type of data is this?
        ///                 |        *     | m_sSrc                  | from whence does it come (community?  node? program? )
        ///                 |        *     | m_sSrcAux               | extra source info
        ///                 |        *     | m_sOriginatingCommunity | from which community?
        ///                 |        *     | m_sKey                  | what
        ///                 |           8  | m_dfTime                | what time was the notification?
        ///                 |           8  | m_dfVal                 | double data
        ///                 |           8  | m_dfVal2                | double data
        ///                 |          *   | m_sVal                  | string data
        /// ----------------|--------------|-------------------------|---------------------
        ///              <variable length> |                         | Total

        // the names of these variables match the variables is `MOOSMsg.cpp` in MOOS-IvP source code.
        const int32_t nLength = *reinterpret_cast<const int32_t*>(message_start);
        // const int32_t nID = *reinterpret_cast<const int32_t*>(message_start + 4);
        const char cMsgType = *(message_start  + 8);
        const char cDataType = *(message_start  + 9);

        // vvv DEBUG vvv DEVEL
        spdlog::trace("        >>> Inspect Message: @{:3d} [{}][{}]", (message_start - buffer), cMsgType, cDataType );
        switch(cMsgType){
            case  'N':  // MOOS_NOTIFY
                // std::cerr << "            >>> MOOS_NOTIFY Packet >>> Continue processing >>>\n";
                break;
            case  'R':  // MOOS_REGISTER
            case  'U':  // MOOS_UNREGISTER
            case  '*':  // MOOS_WILDCARD_REGISTER
            case  '/':  // MOOS_WILDCARD_UNREGISTER
            case  '~':  // MOOS_NOT_SET
            case  'C':  // MOOS_COMMAND
            case  'A':  // MOOS_ANONYMOUS
            case  '.':  // MOOS_NULL_MSG
            case  'i':  // MOOS_DATA
            case  'K':  // MOOS_POISON
            case  'W':  // MOOS_WELCOME
            case  'Q':  // MOOS_SERVER_REQUEST
            case   -2:  // MOOS_SERVER_REQUEST_ID
            case  'T':  // MOOS_TIMING
            case  '^':  // MOOS_TERMINATE_CONNECTION
                // ignore / break
                cursor = message_start + nLength;
                return nullptr;
        }

        switch( cDataType ){
            case 'B':  // BINARY
                // fall-through
            case 'D':  // DOUBLE
                cursor = message_start + nLength;
                return nullptr;
            case 'S':  // STRING
                // fprintf( stderr, "            >> STRING value type   >>> continue >>> \n");
                break;
        }

        cursor = message_start + 10;
        const std::string sSrc = extract_string();
        const std::string sSrcAux = extract_string();
        const std::string sOriginatingCommunity = extract_string();
        const std::string sKey = extract_string();

        // fprintf( stderr, "            ::sSrc:                   (%2lu): %s \n", sSrc.length(), sSrc.c_str() );
        // if( sKey == "MOOSDB_shoreside" ){
        //     std::cerr << "            << PLOGGER_STATUS << Skippping.\n";
        //     cursor = message_start + nLength;
        //     return nullptr;
        // }

        // fprintf( stderr, "            ::sSrcAux:                (%2lu): %s \n", sSrcAux.length(), sSrcAux.c_str() );
        // fprintf( stderr, "            ::sOriginatingCommunity:  (%2lu): %s \n", sOriginatingCommunity.length(), sOriginatingCommunity.c_str() );

        // if( sKey.starts_with("NODE_REPORT")){
        if( sKey == "NODE_REPORT" ){
            // spdlog::trace("        <<<:Extract:Message//Key:{}     src: {}//{}    comm: {}",  sKey, sSrc, sSrcAux, sOriginatingCommunity );
            // pass-through / continue below
        }else{
            if( sKey == "PLOGGER_STATUS" ){
                // spdlog::trace( "            << APPC_REQ* << Skipping." );
            }else if( sKey.starts_with("APPCAST_REQ") ){
                // spdlog::trace( "            << APPC_REQ* << Skipping." );
            }else{
                spdlog::trace( "            <<!!:Unrecognized-Message-Key: {}", sKey );
            }
            // but everything in this category -- we still just silently consume & ignore
            cursor = message_start + nLength;
            return nullptr;
        }

        cursor += 3*sizeof(double);  // skip ahead to the string-value field
        cache.str = extract_string();
        // fprintf( stderr, "                ::string-value:    (%2lu): %s \n", value.length(), value.c_str() );
        cursor = message_start + nLength;

        if( cache.str.empty() ){
            return nullptr;
        }
        return &cache;
    }

    cursor = nullptr;
    return nullptr;
}



}  // namespace moos
}  // namespace parsers
