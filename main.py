import ctypes
helpers = ctypes.CDLL('./helper.so')
robots = [71, 108, 116, 42, 80, 96, 65, 76, 87, 61]
scrooges = [18, 24, 103, 97, 98, 27, 64, 47, 28, 45, 112, 17, 122, 16]
cashbags = [84, 4, 22, 63, 12, 107, 85, 78, 77, 0]
dropspots = [14, 78]
cash_carried = [0, 0, 0, 0, 0]
obstacles = [0, 99, 19, 2, 98, 53, 11, 8]


def convert_list(lst):
    return (ctypes.c_int * len(lst))(*lst)


helpers.get_action(
    convert_list(robots),
    convert_list(scrooges),
    convert_list(cashbags),
    convert_list(dropspots),
    convert_list(cash_carried),
    convert_list(obstacles),
    len(robots),
    len(scrooges),
    len(cashbags),
    len(dropspots),
    len(cash_carried),
    len(obstacles)
)
