#!/usr/bin/python
#_*_coding:utf-8_*_

from xml.dom import minidom
import xlrd, os, sys, re, getopt

field_name_row = 0
var_name_row = 1
type_name_row = 2
data_begin_row = 4
namespace = "tbx"
package_name = "pb"
key_name = "tbxid"

proto_data_type = ( "double", "float", "int32", "int64", "uint32", "uint64", "sint32", "sint64", "bool", "string" )

def get_file_name(filename):
    fpath, fname = os.path.split(filename)
    fname, extname = os.path.splitext(fname)
    return fname

def extract_xml(fname):
    all_sheets = {}
    fp = open(fname, "r")
    content = fp.read()
    fp.close()
    try:
        charset = re.compile(".*\s*encoding=\"([^\"]+)\".*", re.M).match(content).group(1)
    except:
        charset = "UTF-8"
    if charset.upper() != "UTF-8":
        content = re.sub(charset, "UTF-8", content)
        content = content.decode(charset).encode("UTF-8")
    try:
        dom = minidom.parseString(content)
        print "-" * 80
        print "开始解析 %s ..." % fname
    except Exception, e:
        print "xml文件解析错误: %s" % e
    tables = dom.getElementsByTagName("table")
    for table in tables:
        xls_file = table.attributes["name"].value
        print "\tExcel文件:", xls_file
        sheets = table.getElementsByTagName("sheet")
        all_sheets[xls_file] = {}
    	for sheet in sheets:
    	    target_file = sheet.attributes["filename"].value
    	    sheet_name = sheet.attributes["name"].value
            fields = sheet.getElementsByTagName("field")
            all_sheets[xls_file][sheet_name] = {"target":target_file, "data":[]}
            print "\t\tsheet: %s" % sheet_name
    	    for field_node in fields:
                name = field_node.attributes["name"].value
                all_sheets[xls_file][sheet_name]["data"].append({"name":name})
                print "\t\t\tfield: %s" % name
    print "解析%s文件完成...OK" % fname
    print "-" * 80
    return all_sheets

#检查数据类型
def check_proto_type(type_name):
   return (type_name in proto_data_type) 

#通过遍历，搜索一张表的id字段
def get_record_by_id(_id, sheet):
    r = -1
    for i in range(data_begin_row, sheet.nrows):
        val = sheet.cell_value(i, 0)
        if int(val) == _id:
            r = i
            break
    return r

#获取sheet所有的字段
def get_fields_by_sheet(sheet):
    fields = []
    for c in range(sheet.ncols):
        try:
            fields.append({ "name":sheet.cell_value(field_name_row, c), "column":c }) 
        except Exception, e:
            print e
            print fields
    return fields


#递归填充proto数据
def fill_proto_data(obj, sheet, fields, r, book):
    if len(fields) == 0:
        return
    val = sheet.cell_value(r, fields[0]["column"])
    exec "obj.{0} = int(val)".format(key_name)
    sheet_types = get_union_sheet_types(book, sheet, fields)
    for f in fields:
        try:
            var_name = sheet.cell_value(var_name_row, f["column"])
            type_name = sheet.cell_value(type_name_row, f["column"])
            val = sheet.cell_value(r, f["column"])
            if check_proto_type(type_name): 
                if type_name == "float":
                    exec "obj.{0} = float(val)".format(var_name)
                elif type_name == "string":
                    val = str(val).decode("UTF-8")
                    exec "obj.{0} = val".format(var_name)
                else:
                    exec "obj.{0} = int(val)".format(var_name)
            else:
                _sheet = sheet_types[type_name]
                _fields = get_fields_by_sheet(_sheet)
                for _id in str(val).split(":"):
                    _id = int(float(_id))
                    _r = get_record_by_id(_id, _sheet)
                    if -1 != _r:
                        exec "_obj = obj.{0}.add()".format(var_name)                 
                        fill_proto_data(_obj, _sheet, _fields, _r, book)
                    else:
                        print "<{0}>表中，没有找到id={1}的记录".format(type_name, _id)
        except Exception, e:
            print "错误:行:%d,列:%d,列名:%s,列类型%s" % (r+1, f["column"]+1,f["name"],type_name)


#创建tbx文件，写入数据
def create_tbx(mod, all_xls, outDir):
    for xls_file, table in all_xls.items():
        book = table["book"]
        sheet_name = table["sheet_name"]
        sheet = get_sheet(book, sheet_name)
        sheetname_define = table["sheet_prefix"]
        pb = eval("mod." + sheetname_define + "()")
        tbx_filename = os.path.join(outDir, table["tbxfile"])
        print "读取%s中,开始创建%s" % (xls_file,tbx_filename)
        for r in range(data_begin_row, sheet.nrows):
            try:
                obj = pb.datas.add()
                fill_proto_data(obj, sheet, table["fields"], r, book)
            except:
                print "错误:第%d行出现错误,请检查表%s" % ((r+1),xls_file)
        op = file(tbx_filename, "wb")
        op.write(pb.SerializeToString())
        op.close()
        print "\t创建%s...OK\n" % tbx_filename

#创建xml配置文件
def create_xml_config(all_xls, outDir):
    xml_filename = os.path.join(outDir, "tbx.xml")
    xml_fp = file(xml_filename, "wb")
    try:
        xml_fp.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
        xml_fp.write("<tbx>\n")
        for xls_file, table in all_xls.items():
            sheet_name = table["sheet_name"]
            sheetname_define = table["sheet_prefix"]
            xml_fp.write("\t<file name=\"{0}.{1}\" path=\"{2}\"/>\n".format(package_name, sheetname_define, table["tbxfile"]))
        xml_fp.write("</tbx>\n")
    finally:
        xml_fp.close()
        print "创建%s...OK" % xml_filename

#创建cpp头文件
def create_cpp_tbx_header_file(all_xls, cpp_dir):
    load_table_code = ""
    get_table_func_declare = ""
    get_obj_func_declare = ""
    get_table_func_define = ""
    get_obj_func_define = ""
    table_declare_code = ""
    for xls_file, table in all_xls.items():
        sheet_name = table["sheet_name"]
        sheetname_define = table["sheet_prefix"]
        load_table_code += "\t\tload_table(\"{0}.{1}\", _{1}, func);\n".format(package_name, sheetname_define, sheetname_define)
        get_table_func_declare += "\tconst {0}::table< {1}::{2}, {1}::{2}::{3} > &{2}();\n".format(namespace, package_name, sheetname_define, sheet_name)
        get_obj_func_declare += "\tconst {0}::{1}::{2} &{1}(unsigned int id);\n".format(package_name, sheetname_define, sheet_name)
        get_table_func_define += "\tconst {0}::table< {1}::{2}, {1}::{2}::{3} > &{2}() {{\n\t\treturn _{2};\n\t}}\n\n".format(namespace, package_name, sheetname_define, sheet_name)
        get_obj_func_define += "\tconst {0}::{1}::{2} &{1}(unsigned int id) {{\n\t\treturn _{1}.get(id);\n\t}}\n\n".format(package_name, sheetname_define, sheet_name)
        table_declare_code += "\t{0}::table< {1}::{2}, {1}::{2}::{3} > _{2};\n".format(namespace, package_name, sheetname_define, sheet_name)
    filename = os.path.join(cpp_dir, "tbx.h")
    fp = file(filename, "wb")
    try:
        fp.write("#ifndef __TBX_H__\n")
        fp.write("#define __TBX_H__\n")
        fp.write("#include \"tbx.pb.h\"\n")
        fp.write("#include \"tbxbase.h\"\n\n")
        fp.write("namespace {0} {{\n\n".format(namespace))
        fp.write(get_table_func_declare)
        fp.write("\n")
        fp.write(get_obj_func_declare)
        fp.write("}\n\n")
        fp.write("#endif\n\n")
    finally:
        fp.close()
        print "创建%s...OK" % filename
    filename = os.path.join(cpp_dir, "tbx.cpp")
    fp = file(filename, "wb")
    try:
        fp.write("#include \"tbx.h\"\n\n")
        fp.write("namespace {0} {{\n\n".format(namespace))
        fp.write(table_declare_code)
        fp.write("\n")
        fp.write("\tvoid mgr::init(Fir::XMLParser &xml, const {0}::ReadTbxBufFunc &func) {{\n".format(namespace))
        fp.write("\t\tloadconfig(xml);\n")
        fp.write(load_table_code)
        fp.write("\t}\n\n")
        fp.write(get_table_func_define)
        fp.write(get_obj_func_define)
        fp.write("}\n\n")
    finally:
        fp.close()
        print "创建%s...OK" % filename


#根据proto描述文件创建代码
def create_pycode(proto_dir, py_dir, cpp_dir, proto_filename):
    os.system("protoc -I={0} --python_out={1} --cpp_out={2} {3}".format(proto_dir, py_dir, cpp_dir, proto_filename))
    os.system("mv {0}/tbx.pb.cc {0}/tbx.pb.cpp".format(cpp_dir))
    print "创建python与C++代码...OK"
    mod = import_mod("tbx_pb2", py_dir)
    return mod

#import 模块
def import_mod(modname, path):
    if sys.modules.has_key(modname): del sys.modules[modname]
    sys.path.insert(0, path)
    mod = __import__(modname)
    del sys.path[0]
    return mod

#根据sheet名称获取sheet对象
def get_sheet(book, sheet_name):
    try:
        sheet = book.sheet_by_name(sheet_name)
    except Exception, e:
        print "*" * 40
        print "未找到sheet", sheet_name
        print e
    return sheet

#获取一个sheet中所有的联合sheet类型
def get_union_sheet_types(book, sheet, fields):
    sheet_types = {}
    for f in fields:
        _name = sheet.cell_value(type_name_row, f["column"])
        if not check_proto_type(_name):
            try:
                _sheet = get_sheet(book, _name)
            except Exception, e:
                print "未知的数据类型:", _name
            sheet_types[_name] = _sheet 
    return sheet_types

#填充构建proto message 描述
def create_proto_sub_message_code(sheet_name, book, fields, tab_char):
    next_tab_char = "\t" + tab_char
    sheet = get_sheet(book, sheet_name)
    code = "{0}message {1} {{\n\n".format(tab_char, sheet_name)
    sheet_types = get_union_sheet_types(book, sheet, fields)
    for _name, _sheet in sheet_types.items():
        _fields = get_fields_by_sheet(_sheet)
        code += create_proto_sub_message_code(_name, book, _fields, next_tab_char)
    for i, f in enumerate(fields):
        try:
            var_name = sheet.cell_value(var_name_row, f["column"])
            type_name = sheet.cell_value(type_name_row, f["column"])
            var_mode = "required"
            if not check_proto_type(type_name):
                var_mode = "repeated"
            if i == 0:
                code += "{0}\t{1} {2} {3} = {4};\n".format(tab_char, var_mode, type_name, key_name, i + 1)
            code += "{0}\t{1} {2} {3} = {4};\n".format(tab_char, var_mode, type_name, var_name, i + 2)
        except Exception, e:
            print e
            print i, f["name"]
    code += "\n"
    code += tab_char + "}\n\n" 
    return code

#创建proto message的描述代码
def create_proto_message_code(sheet_name, book, fields, sheet_prefix):
    code = "message " + sheet_prefix + " {\n\n"
    code += create_proto_sub_message_code(sheet_name, book, fields, "\t")
    code += "\trepeated " + sheet_name + " datas = 1;\n"
    code += "}\n\n"
    return code

#获取sheet中所需的字段
def get_need_field(book, sheet, fields):
    for f in fields:
        for c in range(sheet.ncols):
            if sheet.cell_value(field_name_row, c) == f["name"]:
                f["column"] = c
                break
    return fields

def extract_excel(all_sheets, inDir, outDir, cppDir):
    proto_dir = inDir
    py_dir = inDir
    cpp_dir = cppDir
    proto_dir = os.path.join(inDir, "proto")
    py_dir = os.path.join(inDir, "py")
    if not os.path.exists(proto_dir): os.mkdir(proto_dir)
    if not os.path.exists(py_dir): os.mkdir(py_dir)
    if not os.path.exists(cpp_dir): os.mkdir(cpp_dir)
    if not os.path.exists(outDir): os.mkdir(outDir)
    all_xls = {}
    proto_filename = os.path.join(proto_dir, "tbx.proto")
    proto_fp = file(proto_filename, "wb")
    try:
        proto_fp.write("package " + package_name + ";\n\n")
        #循环处理所有xls文件
        for xls_file in all_sheets.keys(): 
            xls_filename = os.path.join(inDir, xls_file)                   
            print "开始读取Excel文件...", xls_filename
            book = xlrd.open_workbook(xls_filename)
            #循环处理这个xls文件中的所有sheet
            for sheet_name, table in all_sheets[xls_file].items():
                print "\t开始读取 < %s > 工作表..." % sheet_name
                sheet_prefix = get_file_name(table["target"])
                sheet = get_sheet(book, sheet_name)
                fields = get_need_field(book, sheet, table["data"])
                all_xls[xls_file] = { "book":book,  "sheet_name":sheet_name, "fields":fields, "sheet_prefix":sheet_prefix, "tbxfile":table["target"] }
                print  "\t\t创建 < %s > 的proto描述..." % sheet_name
                proto_fp.write(create_proto_message_code(sheet_name, book, fields, sheet_prefix))
    finally:
        proto_fp.close()
        print "创建所有sheet的proto描述完成...OK"
        print "-" * 80
    #生成代码
    mod = create_pycode(proto_dir, py_dir, cpp_dir, proto_filename)
    create_cpp_tbx_header_file(all_xls, cpp_dir)
    #开始写入数据
    create_tbx(mod, all_xls, outDir)
    #创建xml配置
    create_xml_config(all_xls, outDir)
    print "-" * 80
    os.system("rm -rf " + py_dir + " " + proto_dir)
    
#格式化异常信息
def format_exception(etype, value, tb, limit=None):
    import traceback
    result = ['Tracebak信息:']
    if not limit:
        if hasattr(sys, 'tracebacklimit'):
            limit = sys.tracebacklimit
    n = 0
    while tb and (not limit or n < limit):
        f = tb.tb_frame
        lineno = tb.tb_lineno
        co = f.f_code
        filename = co.co_filename
        name = co.co_name
        locals = f.f_locals
        result.append('  File %s, line %d, in %s' % (filename, lineno, name))
        try:
            result.append('  (Object: %s) ' % locals[co.co_varnames[0]].__name__)
        except:
            pass
        try:
            result.append('  (Info: %s) ' % str(locals['__traceback_info__']))
        except:
            pass
        tb = tb.tb_next
        n += 1
    result.append(' '.join(traceback.format_exception_only(etype, value)))
    return result

def schedule(config, inDir, outDir, cppDir):     
    try: 
        all_sheets = extract_xml(config)
        extract_excel(all_sheets, inDir, outDir, cppDir)
    except:
        print "程序出现异常!"
        traceInfo = format_exception(*sys.exc_info())
        try:
            file = open("except.log","w")
            file.write('\n'.join(traceInfo))
        finally:
            file.close()        
    finally:        
        print "表格打包全部完成...OK"
        
def main():       
    reload(sys)
    sys.setdefaultencoding("utf-8")
    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help","inDir=", "outDir=", "cppDir=", "config="])
    except getopt.GetoptError, err:   
        print str(err)        
        sys.exit(2)
    inDir = "."
    outDir = "."
    cppDir = "."
    config = None
    for o, a in opts:
        if o in ['-h',"--help"]:
            usage()
            sys.exit()
        elif o == "--inDir":
            inDir = a
        elif o == "--outDir":
            outDir = a
        elif o == "--cppDir":
            cppDir = a
        elif o == "--config":
            config = a
        else:
            assert False, "未处理操作"
    try:
        if config is None:
            usage()
            sys.exit()
        else:
            schedule(config, inDir, outDir, cppDir)
    except Exception, e:
        print str(e)

def usage():
    print "usage: \n\t--config=xml配置文件路径\n\t--inDir=Excel文件放置目录\n\t--outDir=输出结果目录\n\t--cppDir=cpp代码生成目录\n"
 
if __name__ == "__main__":
    main()
     
