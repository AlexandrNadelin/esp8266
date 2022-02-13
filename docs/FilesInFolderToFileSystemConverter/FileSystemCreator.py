import sys  # We need sys so that we can pass argv to QApplication
import os
import os.path

def createFileSystem(folderPath: str,filePath: str):
    fileWriteFileSystem = open(filePath,'w')
    if(filePath.endswith('.h')): #arduino variant
        fileWriteFileSystem.write('#ifndef __FILE_SYSTEM_H \r\n')
        fileWriteFileSystem.write('#define __FILE_SYSTEM_H \r\n')

        for root, dirs, files in os.walk(folderPath):
            for file in files:
                fileWriteFileSystem.write('static const unsigned char data_'+file.replace('.','_')+'[]={\r\n')
                f = open(folderPath+'/'+file,'rb')
                content = f.read()
                for i in range(content.__len__()//20):
                    for j in range(20):
                        fileWriteFileSystem.write('0x'+format(content[20*i +j], '02X')+',')
                    fileWriteFileSystem.write('\r\n')
                for j in range(content.__len__()%20):
                    fileWriteFileSystem.write('0x'+format(content[20*i +j], '02X')+',')
                fileWriteFileSystem.write('};\r\n\r\n')
                fileWriteFileSystem.write('static const int size_'+file.replace('.','_')+' = '+str(content.__len__())+';\r\n\r\n')
                f.close()

        fileWriteFileSystem.write('#endif \r\n\r\n\r\n')
    else: #stm variant
        fileWriteFileSystem.write('#include "FileSystem.h"\r\n')
        fileWriteFileSystem.write('#include "string.h"\r\n')
        fileWriteFileSystem.write('#include "fsdata.h"\r\n\r\n')
        fileWriteFileSystem.write('#define file_NULL (struct fsdata_file *) NULL\r\n')
        previousFile: str = 'file_NULL'
        fileCount: int = 0

        for root, dirs, files in os.walk(folderPath):
            for file in files:
                fileCount+=1
                fileWriteFileSystem.write('static const unsigned char name_'+file.replace('.','_')+'[]={\r\n')
                bytes = file.encode()
                for i in range(file.__len__()):
                    fileWriteFileSystem.write('0x'+format(bytes[i], '02X')+',')
                fileWriteFileSystem.write('0x'+format(0x00, '02X')+',')
                fileWriteFileSystem.write('};\r\n\r\n')                        

                fileWriteFileSystem.write('static const unsigned char data_'+file.replace('.','_')+'[]={\r\n')
                f = open(folderPath+'/'+file,'rb')
                content = f.read()
                for i in range(content.__len__()//20):
                    for j in range(20):
                        fileWriteFileSystem.write('0x'+format(content[20*i +j], '02X')+',')
                    fileWriteFileSystem.write('\r\n')
                for j in range(content.__len__()%20):
                    fileWriteFileSystem.write('0x'+format(content[20*i +j], '02X')+',')
                fileWriteFileSystem.write('};\r\n\r\n')
                fileWriteFileSystem.write('const struct fsdata_file file_'+file.replace('.','_')+'[]={{\r\n')
                fileWriteFileSystem.write(previousFile+',\r\n')
                previousFile = 'file_'+file.replace('.','_')
                fileWriteFileSystem.write('name_'+file.replace('.','_')+',\r\n')
                fileWriteFileSystem.write('data_'+file.replace('.','_')+',\r\n')
                fileWriteFileSystem.write(str(content.__len__())+',\r\n')
                fileWriteFileSystem.write('1,\r\n')
                fileWriteFileSystem.write('}};\r\n\r\n')
                f.close()

        fileWriteFileSystem.write('#define FS_ROOT '+previousFile+'\r\n')
        fileWriteFileSystem.write('#define FS_NUMFILES '+str(fileCount)+'\r\n\r\n')
        fileWriteFileSystem.write('err_t fs_open(struct fs_file *file, const char *name){\r\n')
        fileWriteFileSystem.write('  const struct fsdata_file *f;\r\n\r\n')
        fileWriteFileSystem.write('  for(f = FS_ROOT; f != NULL; f = f->next){\r\n')
        fileWriteFileSystem.write('    if(memcmp(name, (const char *)f->name,strlen((const char *)f->name))==0){\r\n')
        fileWriteFileSystem.write('      file->data = (const char *)f->data;\r\n')
        fileWriteFileSystem.write('      file->len = f->len;\r\n')
        fileWriteFileSystem.write('      file->index = f->len;\r\n')
        fileWriteFileSystem.write('      file->pextension = NULL;\r\n')
        fileWriteFileSystem.write('      file->flags = f->flags;\r\n')
        fileWriteFileSystem.write('      return ERR_OK;\r\n')
        fileWriteFileSystem.write('    }\r\n')
        fileWriteFileSystem.write('  }\r\n')
        fileWriteFileSystem.write('  return ERR_VAL;\r\n')
        fileWriteFileSystem.write('}\r\n\r\n\r\n')

    fileWriteFileSystem.close()

    fileProperties = open('properties','w')
    fileProperties.write(folderPath+'/\n')
    fileProperties.write(filePath+'\n')
    fileProperties.close()

