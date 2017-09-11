#include <iostream>
#include <winsock2.h>
#include "md5.h"
 
using std::cout; using std::endl;

int main(void)
{
  char recvBuff[1024] = "";
  char sendBuff[1024] = ""; 
  struct sockaddr_in serv_addr;

  WSADATA wsa;
  SOCKET sockfd;
  struct sockaddr_in server;
  
  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
  {
    printf("Failed. Error Code : %d",WSAGetLastError());
    return 1;
  }
  //Create a socket
  if((sockfd = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
  {
    printf("\n Error : Could not create socket \n");
  }
  printf("Socket created.\n");
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons(8081);
  //Connect to remote server
  if (connect(sockfd , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
    puts("\n Error : Connect Failed \n");
    return 0;
  }
  puts("Connected");

  //char massage[1024] = "";
  //printf("Get file from server : ");
  //fgets(massage,1024,stdin);
  //strcpy(sendBuff, massage);
  //write(sockfd, sendBuff, sizeof(sendBuff)-1);
  recv(sockfd, recvBuff, sizeof(recvBuff)-1,0);
  printf("reciving %s\n",recvBuff);
  char outputPath[1024] = "output/";
  strcat(outputPath, recvBuff);
  printf("saving file to \"%s\"\n",outputPath);
  recv(sockfd, recvBuff, sizeof(recvBuff)-1,0);
  long int fileSize = atol(recvBuff);
  printf("File size: %ld bytes\n",fileSize);
  FILE *outputFile = fopen(outputPath, "wb");
  long int bytesReceived = 0;
  long int totalReceived = 0;
  //unsigned char md5[MD5_DIGEST_LENGTH];
  //MD5_CTX ctx;
  //MD5_Init(&ctx);
  MD5 md5;
  while(totalReceived<fileSize)
  {
    bytesReceived = recv(sockfd, recvBuff, 1, 0);
    fwrite(recvBuff, bytesReceived, 1, outputFile);
    totalReceived += bytesReceived;
    //MD5_Update(&ctx, recvBuff, 1);
    md5.update(recvBuff,1);
    //printf("Reciving %ld/%ld\n",totalReceived,fileSize);
  }
  //MD5_Final(md5, &ctx);
  md5.finalize();
  fclose(outputFile);
  
  char md5sv[32];
  recv(sockfd, recvBuff, sizeof(recvBuff)-1,0);
  strcpy(md5sv, recvBuff);
  md5sv[32] = '\0';
  //closesocket(sockfd);
  printf("md5[sv]: %s\n",md5sv);
  
  //char result_md5[32] = "";
  //char temp[2] = "";
  //for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
  //  sprintf(temp, "%02x", md5[i]);
  //  strcat(result_md5,temp);
  //}
  char result_md5[32];
  strcpy(result_md5,md5.hexdigest().c_str());
  printf("md5[cl]: %s\n",result_md5);
  return 0;
}

