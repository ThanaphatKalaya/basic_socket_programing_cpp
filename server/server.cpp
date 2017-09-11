#include <iostream>
#include <winsock2.h>
#include "md5.h"

using std::cout; using std::endl;

int main(void)
{
  char sendBuff[1024];
  char recvBuff[1024];
  int port = 8081;
  WSADATA WSAData;
  SOCKET listenfd, connfd;
  SOCKADDR_IN serverAddr, clientAddr;
  WSAStartup(MAKEWORD(2,0), &WSAData);
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("socket retrieve success\n");
  serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	bind(listenfd, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
  listen(listenfd, 5);
  printf("listen to port %d\n",port);
  
  while(1)
    {
      int clientAddrSize = sizeof(clientAddr);
	    connfd = accept(listenfd, (SOCKADDR *)&clientAddr, &clientAddrSize);
      //read(connfd, recvBuff, sizeof(recvBuff)-1);
      //strcpy(sendBuff, recvBuff);
      char input[1024] = "ubuntu-16.04.3-desktop-amd64.iso";
      strcpy(sendBuff, input);
      printf("Sending %s\n",input);
      send(connfd, sendBuff, sizeof(sendBuff)-1,0);
      FILE *inputFile = fopen(input, "rb");
      fseek(inputFile, 0, SEEK_END);
      long int fileSize = ftell(inputFile);
      printf("File size: %ld bytes\n",fileSize);
      sprintf(sendBuff, "%ld", fileSize);
      send(connfd, sendBuff, sizeof(sendBuff)-1,0);
      long int pointer,remain;
      //unsigned char md5[MD5_DIGEST_LENGTH];
      //MD5_CTX ctx;
      //MD5_Init(&ctx);
      MD5 md5;
      fseek(inputFile, 0, SEEK_SET);
      int bytesRead;
      if (fileSize < 1024){
        bytesRead = fread(sendBuff, 1, fileSize, inputFile);
        //MD5_Update(&ctx, sendBuff, fileSize);
        md5.update(sendBuff,fileSize);
      }else{
        bytesRead = fread(sendBuff, 1, 1024, inputFile);
        //MD5_Update(&ctx, sendBuff, 1024);
        md5.update(sendBuff,1024);
      }
      while(!feof(inputFile)){
        send(connfd, sendBuff, bytesRead, 0);
        pointer = ftell(inputFile);
        remain = fileSize - pointer;
        if(remain==0){
          bytesRead = fread(sendBuff, 1, 1, inputFile);
          //MD5_Final(md5, &ctx);
          md5.finalize();
        }else if(remain<1024){
          bytesRead = fread(sendBuff, 1, remain, inputFile);
          //MD5_Update(&ctx, sendBuff, remain);
          md5.update(sendBuff,remain);
        }else{
          bytesRead = fread(sendBuff, 1, 1024, inputFile);
          //MD5_Update(&ctx, sendBuff, 1024);
          md5.update(sendBuff,1024);
        }
      }
      fclose(inputFile);
      //char result_md5[32] = "";
      //char temp[2] = "";
      //for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
      //  sprintf(temp, "%02x", md5[i]);
      //  strcat(result_md5,temp);
      //}
      //write(connfd, result_md5, 32);
      char result_md5[32];
      strcpy(result_md5,md5.hexdigest().c_str());
      send(connfd, result_md5, 32,0);
      printf("md5[sv]: %s\n",result_md5);
      closesocket(connfd);
      printf("Send finished\n");
    }

  return 0;
}

