import struct


def WriteIndex(fileObj, listData):
    off = 0
    for it in listData:
        fileObj.write(struct.pack("<H", off))
        off += len(it)


def WriteData(fileObj, listData):
    for it in listData:
        # 将列表中的整数元素转换为uint8_t格式
        packed_data = struct.pack(f"{len(it)}B", *it)
        # 写入数据
        fileObj.write(packed_data)


def WriteListToFontBinFile(fileFullName, listData):
    # 定义文件头结构体格式 B表示一个字节的无符号整数，H表示两个字节的无符号整数，L表示四个字节的无符号整数，Q表示八个字节的无符号整数
    format_string = '<4s4sLHH'
    # 创建结构体对象
    name = b"font"  # 文件格式名
    compress = b""  # 压缩算法，没有则为空
    crc = 0  # 数据校验码
    ver = 1  # 格式版本号 1~255
    indexNum = len(listData)  # 索引个数
    # 后面紧跟索引表 单个索引item长度为uint16_t
    # 后面紧跟数据区 单个数据为变长
    header = struct.pack(format_string, name, compress, crc, ver, indexNum)  # 生成文件头
    # 打开文件（以二进制写入模式）
    with open(fileFullName, 'wb') as f:
        # 写入文件头
        f.write(header)
        # 计算并写入索引
        WriteIndex(f, listData)
        # 写入数据
        WriteData(f, listData)

# unit test
# data = [[0xe6, 0x97, 0xb6], [0xe5, 0xa4, 0x9a, 0xe4, 0xba, 0x91], [0x51, 0x51]]
# WriteListToFontBinFile("output.bin", data)
