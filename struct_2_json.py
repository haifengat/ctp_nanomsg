#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
__title__ = ''
__author__ = 'HaiFeng'
__mtime__ = '20180123'
"""

import json

class Struct2Json(object):
    """"""

    def __init__(self):
        """"""
        self.defines = {}
        self.enums = {}

    def exchange(self, path='ctp_20171123/'):
        """"""
        lines = []
        with open(path + 'ThostFtdcUserApiDataType.h', 'r') as fstruct:
            lines = fstruct.readlines()
        for i in range(0, len(lines)):
            line = lines[i]
            
            if line.find('typedef') >= 0:
                line = line.replace('typedef', '') # .replace(' ', '').replace('\t', '')
                if line.find(' int ') >= 0:
                    self.defines[line.split(' int ')[1][:-2]] = 'int'
                elif line.find(' double ') >= 0:
                    self.defines[line.split(' double ')[1][:-2]] = 'double'
                elif line.find(' char ') >= 0:
                    name = line.split(' char ')[1][:-2] # trim the end ;\n
                    if name.find('[') >= 0: # 类型定义
                        name = name.split('[')[0]
                        self.defines[name] = 'string'
                    else: # 枚举
                        remark = ''
                        items = ''
                        for j in range(i-1, 0, -1):
                            # 枚举项
                            if lines[j].find('#define') >= 0: # 多空格处理
                                item = '''{0} = {1}'''.format(lines[j].split(' ')[1], ord(lines[j].split(' ', 2)[2].replace(' ','')[1]))
                                if lines[j-1].find('///') == 0: # 枚举项注释
                                    comment = '"""{0}"""'.format(lines[j-1].replace('/', '')).replace('\n', '')
                                    item = '\t{0}\n\t{1}\n\t{0}\n'.format(comment, item)
                                items = item + items
                            # 枚举注释
                            elif lines[j].find(name.replace('TThost', '')) >= 0:
                                remark = lines[j][lines[j].find('是一个')+3:-3]
                                break
                        # class ActionFlagType(Enum):
                        #     """操作标志类型"""
                        #     Delete = 48
                        #     Modify = 51
                        self.enums[name] = '''
class {0}(Enum):
    """{1}"""
{2}'''.format(name, remark, items)

        # print(str(self.defines))
        # print(str(self.enums))
        with open('enums.py', 'w', encoding='utf-8') as f:
            # json.dump(self.enums, f, ensure_ascii=False)
            f.write('''#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
__title__ = ''
__author__ = 'HaiFeng'
__mtime__ = '20180123'
"""

''')
            for item in self.enums:
                f.write(self.enums[item])
        with open('defs.py', 'w', encoding='utf-8') as f:
            json.dump(self.defines, f, ensure_ascii=False, indent=4)



if __name__ == '__main__':
    sj = Struct2Json()
    sj.exchange()
    # input()
