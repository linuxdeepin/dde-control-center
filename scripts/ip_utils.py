#!/usr/bin/env python
#-*- coding:utf-8 -*-

import re
from socket import inet_aton, inet_ntoa
from struct import unpack, pack

def _check_ip(ip_add):
    """
    common func
    """
    p = re.compile(r'^(([01]?[\d]{1,2})|(2[0-4][\d])|(25[0-5]))' \
        r'(\.(([01]?[\d]{1,2})|(2[0-4][\d])|(25[0-5]))){3}(\/(\d+))?$')

    return p.search (str (ip_add)) is not None

def isValidIP(ip_add):
    """
    Return the validity of the IP

    >>>eisoopylib.isValidIP("192.168.0.1")
    True
    >>>eisoopylib.isValidIP("192.168.0")
    False
    >>>eisoopylib.isValidIP("test")
    False
    >>>eisoopylib.isValidIP("10.0.0.256")
    False

    etc.
    """
    if _check_ip(ip_add):
        return True
    return False

def isValidMask(mask):
    """
    Return the validity of the mask

     >>>eisoopylib.isValidMask("255.255.255.0")
    True
    >>>eisoopylib.isValidMask("192.168.0")
    False
    >>>eisoopylib.isValidMask("test")
    False
    >>>eisoopylib.isValidMask("0.0.0.0")
    False
    >>>eisoopylib.isValidMask("255.255.255.255")
    True

    etc.
    """
    try:
        if _check_ip(mask):
            mask_num, = unpack("!I", inet_aton(mask))
            if mask_num == 0:
                return False

            # get inverted
            mask_num = ~mask_num + 1
            binstr = bin (mask_num)[3:]
            # convert to positive integer
            binstr = '0b%s' % ''.join('1' if b == '0' else '0' for b in binstr)
            mask_num = int (binstr, 2) + 1
            # check 2^n
            return (mask_num & (mask_num - 1) == 0)
        return False
    except Exception:
        return False

def isUseableIP(ip_add, mask=None):
    """
    Return the availability of the IP
    
    >>>eisoopylib.isUseableIP("192.168.0.1", "255.255.255.0")
    True
     >>>eisoopylib.isUseableIP("127.0.0.1") //Loopback address
    False
    >>>eisoopylib.isUseableIP("224.0.0.1") //Multicast address(224.0.0.0 - 239.255.255.255)
    False
    >>>eisoopylib.isUseableIP("169.254.0.1") //Failed dhcp allocation IP(169.254.x.x)
    False
    >>>eisoopylib.isUseableIP("192.168.77.128", "255.255.255.128") //Network number is 1
    False

    etc.
    """
    if _check_ip(ip_add):
        ip_split = ip_add.split('.')
        # 如果IP地址以0开头，则不可用
        if ip_split[0] == '0':
            return False
        # 如果IP地址以255开头，则不可用
        if ip_split[0] == '255':
            return False
        # 如果IP地址以127开头，则不可用
        if ip_split[0] == '127':
            return False
        # 如果IP地址以169.254开头，则不可用
        if ip_split[0] == '169' and ip_split[1] == '254':
            return False

        ip_num = ip2int(ip_add)
        # 2进制字符串，左补零，共32位
        ip_bit = bin(ip_num)[2:].zfill(32)
        # 过滤全零地址
        if ip_num == 0:
            return False
        # 如果是A类地址，则掩码为255.0.0.0
        if ip_bit[0] == '0':
            mask = mask or "255.0.0.0"
        # 如果是B类地址，则掩码为255.255.0.0
        elif ip_bit[:2] == '10':
            mask = mask or "255.255.0.0"
        # 如果是C类地址，则掩码为255.255.255.0
        elif ip_bit[:3] == '110':
            mask = mask or "255.255.255.0"
        # 其余地址全部不可用
        else:
            return False

        # 掩码不合法则不可用
        if not isValidMask(mask):
            return False

        # 根据掩码计算子网地址，如果IP为子网地址，则不可用
        subnet = calcSubnet (ip_add, mask)
        if ip_add == subnet:
            return False
        # 根据子网以及掩码计算广播地址，如果IP为广播地址，则不可用
        if ip_add == calcBroadcastBySubnet (subnet, mask):
            return False

        return True
    else:
        return False


def ip2int(ip_add):
    """
    Return the decimal number of the IP

    >>>eisoopylib.ip2int("192.168.0.1")
    3232235521

    etc.
    """
    try:
        if _check_ip(ip_add):
            result = unpack("!I", inet_aton(ip_add))
            return result[0]
        else:
            return False
    except ValueError:
        return False

def int2ip(int_num):
    """
    Return the IP of the valid decimal number

    >>>eisoopylib.int2ip(3232235521)
    192.168.0.1

    etc.
    """
    try:
        return inet_ntoa (pack ("!I", int_num))
    except Exception:
        return False

def calcSubnet(ip_add, mask):
    """
    Return the sub net of the network

    >>>eisoopylib.calcSubnet("192.168.0.1", "255.255.255.0")
    192.168.0.0

    etc.
    """
    if _check_ip(ip_add) and _check_ip (mask):
        ip_num, = unpack("!I", inet_aton(ip_add))
        mask_num, = unpack("!I", inet_aton(mask))
        subnet_num = ip_num & mask_num
        return inet_ntoa (pack ("!I", subnet_num))
    else:
        return False

def calcHostNum(mask):
    """
    Return the host numbers of the network

    >>>eisoopylib.calcHostNum("255.255.255.0")
    254

    etc.
    """
    try:
        if isValidMask(mask):
            bit_num = bin (ip2int (mask)).count('1')

            return (2 ** (32 - bit_num)) - 2
        return False
    except Exception:
        return False

def isInSameNetwork (ip_add1, ip_add2, mask):
    """
    Return ip_add1 and ip_add2 in same network

    >>>eisoopylib.isInSameNetwork("192.168.77.1", "192.168.77.2", "255.255.255.0")
    True

    >>>eisoopylib.isInSameNetwork("192.168.77.1", "192.168.8.2", "255.255.0.0")
    True

    >>>eisoopylib.isInSameNetwork("192.168.77.1", "192.168.8.2", "255.255.255.0")
    False

    """
    if _check_ip (ip_add1) and _check_ip (ip_add2) and _check_ip (mask) \
        and isValidMask (mask):
        ip1_num, = unpack("!I", inet_aton(ip_add1))
        ip2_num, = unpack("!I", inet_aton(ip_add2))
        mask_num, = unpack("!I", inet_aton(mask))
        if ip1_num & mask_num != ip2_num & mask_num:
            return False
        else:
            return True

def calcBroadcast (ip_add, mask):
    """
    Return the broadcast
    >>>eisoopylib.calcHostNum("192.168.77.12", "255.255.255.128")
    192.168.77.127
    """
    subnet = calcSubnet (ip_add, mask)
    if not subnet:
        return False

    return calcBroadcastBySubnet (subnet, mask)

def calcBroadcastBySubnet (subnet, mask):
    """
    Return the broadcast
    >>>eisoopylib.calcHostNum("192.168.77.12", "255.255.255.128")
    192.168.77.127
    """
    if not isValidMask(mask):
        return False

    try:
        subnet_num = ip2int (subnet)

        # calc host bit num
        host_bit = bin (ip2int (mask)).count('1')

        # replace 32 - host_bit numbers 0 to 1
        binstr = ''
        if host_bit < 32:
            binstr = bin (subnet_num)[host_bit-32:]

        binstr = ''.join ('1' for b in binstr)
        binstr = ''.join ([bin (subnet_num)[:host_bit + 2], binstr])

        broadcast_num = int (binstr, 2)
        return int2ip (broadcast_num)
    except Exception:
        return False

def isNetConflict(ip_addr1, mask1, ip_addr2, mask2):
    """
    Return two networks confliction
    >>>eisoopylib.isNetConflict("192.168.77.1", "255.255.255.0", "192.168.77.2", "255.255.255.0")
    False
    """
    subnet1 = calcSubnet (ip_addr1, mask1)
    if not subnet1:
        return False

    subnet2 = calcSubnet (ip_addr2, mask2)
    if not subnet2:
        return False

    if subnet1 == subnet2:
        return False
    
def getPrivateType(ip_addr):
    value = ip2int(ip_addr)
    if value >= ip2int("10.0.0.0") and value <= ip2int("10.255.255.255"):
        return "A"
    elif value >= ip2int("172.16.0.0") and value <= ip2int("172.31.255.255"):
        return "B"
    elif value >= ip2int("192.168.0.0") and value <= ip2int("192.168.255.255"):
        return "C"
    else:
        return ""

def getDefaultMask(ip_addr):
    private_type = getPrivateType(ip_addr)
    if private_type == "A":
        return "255.0.0.0"
    elif private_type == "B":
        return "255. 255.0.0"
    elif private_type == "C":
        return "255.255.255.0"
    else:
        return ""
        
if __name__ == "__main__":
    print getDefaultMask("192.168.1.1")
