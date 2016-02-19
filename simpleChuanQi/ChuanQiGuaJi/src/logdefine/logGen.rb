#!/bin/env ruby

load "logXML.rb"

def fieldFormat(n, fieldType, fieldName, fieldDesc)
	"\t"*n + fieldType + "\t\t" + fieldName + ";\t\t//#{fieldDesc}\n"
end


def getVarg( fieldInfo )

	kv = { "BYTE"=> "%u", "WORD"=> "%u", "DWORD"=> "%u", "INT32" => "%d", "DATETIME"=>"%u", "UINT64"=>"%llu", "INT64"=>"%lld" }

	dataType = fieldInfo[:type]

	a = ( kv.has_key? dataType) ? kv[dataType] : "%s"
	b = ( dataType =~ /VARCHAR|BLOB/ ) ? fieldInfo[:name] : "p->"+fieldInfo[:name]
	return a+",", b+","

end


def getVarg2( fieldInfo, withChild )

	kv = { "BYTE"=> "%u", "WORD"=> "%u", "DWORD"=> "%u", "INT32" => "%d", "DATETIME"=>"%s", "UINT64"=>"%llu", "INT64"=>"%lld" }
	kv.default = "%s"

	dataType = fieldInfo[:type]
	
	fullName = ( withChild ? "pChild[n]." : "p->" ) + fieldInfo[:name]

	b = if dataType =~ /VARCHAR_(\d+)/
			"EscapeLogStr2( #{fullName} ).c_str()"
		elsif dataType =~ /BLOB_(\d+)/
			len = $1
			"PRINT_BLOB2( #{fullName}, #{len} ).c_str()"
		elsif dataType =~ /DATETIME/
			"GetLogTime( (time_t)#{fullName} ).c_str()"
		else
			"#{fullName}"
		end

	return kv[dataType]+",", b+","

end


def printField( n, fieldInfo )
#		{ :name=>"zoneID", 		:type=>"BYTE",		:desc=>"频道ID" },

	if fieldInfo[:member] then

		str = fieldFormat(n, "int", "childCount", "" )
		str += "\n"
		str += "\t"*n +	"struct __CHILD\t{\n"

		fieldInfo[:member].each { |member|
			str += printField(n+1, member)
		}

		str += "\t"*n + "};\n\n"
		return str

	end

	return fieldFormat(n, "char", fieldInfo[:name]+"[#{$1?$1:$2}]", fieldInfo[:desc] ) if fieldInfo[:type] =~ /VARCHAR_(\d+)|BLOB_(\d+)/
	
	type = fieldInfo[:type] == "DATETIME" ? "DWORD"
			: fieldInfo[:type] == "UINT64" ? "__int64"
			: fieldInfo[:type]

	fieldFormat(n, type, fieldInfo[:name], fieldInfo[:desc] )

end

def getStructName(log)
	"LOG_#{log[:name].upcase}"
end

def getEnumName(log)
	"E_LOG_#{log[:name].upcase}"
end


def getLogDefine( log )

	structName = getStructName(log)#"LOG_#{log[:name].upcase}"

	str = "//"+log[:desc]+"\n//"+log[:comment]+"\n"
	str += "struct #{structName}\n{\n"

	str += <<END
	WORD wLen;
	WORD wCmdType;

END

	log[:member].each { |field|

		str += printField( 1, field )
	}

	str += "\t#{structName} () { memset(this,0,sizeof(*this)); }\n"
	str += "\t#{structName} (const #{structName}& rhs) { memcpy(this,&rhs,sizeof(*this)); }\n"

	str += "};\n//==================================================================\n\n"
end

def getPrintfFormat( fieldInfo )
#		{ :name=>"zoneID", 		:type=>"BYTE",		:desc=>"频道ID" },

	if fieldInfo[:member] then

		strFormat=""
		strData=""

		fieldInfo[:member].each { |member|
			x, y = getPrintfFormat( member )
			strFormat +=x
			strData +=y
		}

		return strFormat, strData
	end

	return getVarg( fieldInfo )

end


def getPrintfFormat2( log  )

#只支持一层子结构,并且子结构必须放在最后

	strFormat=""
	strData=""

	log[:member].each { |member|
		
		if member.has_key? :member 

			member[:member].each { |m|
				x,y = getVarg2( m, "withChild" )
				strFormat +=x
				strData +=y
			}

		else
			x, y = getVarg2( member, nil )
			strFormat +=x
			strData +=y

		end

	}

	return strFormat, strData

end


def getLogCase( log )

	caseName = getEnumName(log) 
	structName = getStructName(log)

	a, b = "", ""
	a, b = getPrintfFormat2( log )
	a.chop!
	b.chop!
#只支持一层子结构,并且子结构必须放在最后
	head = <<END
	case #{caseName}:
	{
		#{structName}* p = ((#{structName}*)data);
END

	str = 
	if log[ :member ][ -1 ].has_key? :member 
		<<END
		BREAK_IF_CHILD(#{structName})
		#{structName}::__CHILD* pChild = (#{structName}::__CHILD*)( (((char*) p) +sizeof(#{structName})) );
		for(int n=0;n<p->childCount;n++)
		{
			xfprintf(fp, "%d,#{a}\\n", #{caseName}, #{b});
		}
		break;
	}

END

	else
		<<END
		BREAK_IF(#{structName})
		xfprintf(fp, "%d,#{a}\\n", #{caseName}, #{b});
		break;
	}

END

	end

	return head + str
	#puts "===================================================="
end


def getLogEnum( log  )
	"\t#{getEnumName(log)} = #{log[:cmdtype]},\t\t//#{log[:desc]}\n"
end

def getSqlField( type )
		case type

			when "UINT64"
				"BIGINT UNSIGNED NOT NULL DEFAULT 0"

			when "DWORD"
				"INT UNSIGNED NOT NULL DEFAULT 0"

			when "WORD"
				"SMALLINT UNSIGNED NOT NULL DEFAULT 0 "

			when "BYTE"
				"TINYINT UNSIGNED NOT NULL DEFAULT 0"

			when "DATETIME"
				"DATETIME "
			
			when /VARCHAR_(\d+)/
				"VARCHAR(#{$1}) BINARY DEFAULT ''"

			when /BLOB_(\d+)/
				"BLOB"

			else
				""
		end
end


def getCreateTbl( log )

	str = "CREATE TABLE IF NOT EXISTS `#{log[:name]}` (\n"

	log[ :member ].each { |member|

		if ( member.has_key? :member ) then
			member[:member].each { |m|
				str += "\t`#{m[:name]}` " + getSqlField( m[:type] ) + " COMMENT '#{m[:desc]}',\n"
			}
		else
			str += "\t`#{member[:name]}` " + getSqlField( member[:type] ) + " COMMENT '#{member[:desc]}',\n"
		end
	}

	str.chop!
	str.chop!
	str += "\n)ENGINE=InnoDB DEFAULT CHARSET=latin1;\n\n"

end


logCase = ""
logDefine = "#pragma pack(push, 1)\n\n"
logEnum = "enum E_LOG_CMD\n{\n\tE_LOG_FIRST = 80,\n"
logDB = "SET FOREIGN_KEY_CHECKS=0;\n"

GenList.each { |log|
	logCase += getLogCase( log )
	logDefine += getLogDefine( log )
	logEnum += getLogEnum( log )
	logDB += getCreateTbl( log )
}

logEnum += "\tE_LOG_LAST,\n};\n\n"
logDefine += "#pragma pack(pop)\n"

open("logCase.hpp", "wb").write( logCase )
open("logDefine.hpp", "wb").write( (logEnum + logDefine).encode("GBK") )
open("logDB.sql", "wb").write( logDB )
#puts logDB

