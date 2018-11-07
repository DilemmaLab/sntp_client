#include  <math.h>  
#include  <string.h>   
#include  <stdio.h>  
#include  <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>  

  main() {  
	ntpdate();
  }
  ntpdate() {  
	char  *hostname="91.226.136.136";
  	int  portno=123; //NTP- порт 123  
	int  maxlen=1024; // размер буфера 
	int  i;
     	/*  Построение сообщения длиной 48 байт, в котором все байты = 0, кроме  версии протокола 100, т.е. 4 */  
	unsigned char  msg[48]={100,0,0,0,0,0,0,0,0}; //  отправляемый пакет 
	unsigned  long  buf[ maxlen]; //  буфер приема 
	struct protoent *proto;
  	struct sockaddr_in server_addr;
  	int  s; //  соккет 
	int    tclock1,  tclock2=0; //  время на компьютере 
	int  tmit; //  время сервера 
	proto=getprotobyname("udp");
  	s=socket(PF_INET, SOCK_DGRAM, proto- >p_proto);
  	memset( &server_addr, 0, sizeof( server_addr ));
  	server_addr.sin_family=AF_INET;
  	server_addr.sin_addr.s_addr = inet_addr(hostname);
  	server_addr.sin_port=htons(portno);
  	tclock1 =clock(); //Время на компьютере до отправки сообщения 
	printf("Clock before sending request = %d\ n",tclock1); 
	//  Отправка данных 
	i=sendto(s,msg,sizeof(msg),0,(struct sockaddr  *)&server_addr,sizeof(server_addr));
  	tclock2=clock(); //Время на компьютере после отправки сообщения 
	printf("Clock after sending request = %d\ n",tclock2);
  	printf("Program spent %d ms on request (RTT)\ n",tclock2- tclock1);
  	//  получение ответных данных 
	i=recv( s,buf,   sizeof( buf),0);
  	printf("Socket descriptor %d\ n",s);
  	printf("recvfr: %d\ n",i);
  	perror ("recvfr");
  	//  Интерпретируем полученное сообщение 
	//  Мы получили 12 строк в буфере приема ,  которые соответствуют формату  SNTP- сообщения без ключа идентификации и дайджест сообщения 
	tmit=ntohl((time_t)buf[10]);
  	int updtm;
  	updtm=ntohl((time_t)buf[4]);
  	updtm- = 2208988800U;
  	printf("Reference Timestamp: %s",(char*)ctime(&updtm));
  	updtm=ntohl((time_t)buf[5]);
  	updtm- = 2208988800U;
  	printf("Originate Timestamp: %s",(char*)ctime(&updtm));
  	updtm=ntohl((time_t)buf[6]);
  	updtm- = 2208988800U;
  	printf("Receive Timestamp: %s",(char*)ctime(&updtm));
  	updtm=ntohl((time_t)buf[7]);
  	updtm- = 2208988800U;
  	printf("Transmit Timestamp: %s",(char*)ctime(&updtm));
  	// Первая строка буфера приема  - интерпретация при помощи побитовых  операций 
	int val;
  	val=( ( buf[0] ) >> 30);
  	switch(val)  {  case 0: printf("No Warning LI=%d\ n", val);
 	break;
  	case 1: printf("Last Minute is 61 sec LI=%d\ n", val);
 		break;
  	case 2: printf("Last Minute is 59 sec LI=%d\ n", val);
 		break;
  	default: printf("Leap Indicator Alarm -  time is not synchronised!  LI=%d\ n", val);
 		break;
  }
  val= buf[0];
  val= ( ( (val) >> 27)&7 );
  printf("Version: %d\ n",(int)val);
  val= buf[0];
  val=( ( val >> 24)& 7 );
  printf("Mode ");
  switch(val)  {  case 0: printf("Reserved = %d\ n", val);
                        break;
                  case 1: printf("Simmetric Active = %d\ n", val);
                        break;
                  case 2: printf("Simmetric Passive = %d\ n", val);
                        break;
                  case 3: printf("Client = %d\ n", val);
                        break;
                  case 4: printf("Server = %d\ n", val);
                        break;
                  case 5: printf("(Wide) BroadCast = %d\ n", val);
                        break;
                  case 6: printf("Reserved for NTP Messeges = %d\ n", val);
                        break;
                  case 7: printf("Reserved for Private Use = %d\ n", val);
                        break;
                  default: printf("Unknown Mode Mode = %d\ n", val);
                        break;
  }
  val= buf[0];
  val = ( ( val >> 16 ) & 255);
  printf ("Stratum = %d -  ", val);
  if(val == 0) printf ("Stratum Death Kiss.Unspecified Server\ n");
  else if(val == 1) printf("Stratum.Primary Reference\ n");
  else if(val <= 15) printf("Stratum.Secondary Reference\ n");
  else if(val <= 255) printf("Stratum.Reserved\ n");
  else printf ("Stratum Unknown\ n");
  float prec = ( ( buf[0] ) & 255);
  printf ("Precision = %f ms\ n", prec);
  float delay;
  delay=ntohl((float)buf[1]);
  printf("Delay = %f ms\ n",delay);
  /*  Конвертация времени  NTP- сервера в стандартное время  Unix/ Linux- системы  - для этого необходимо 
  вычесть из полученного времени число  секунд между  0000  UT   1  Января 1900  ( точка отсчета времени  NTP- сервера ) 
  и  0000  UT   1  Января  1970  ( точка отсчета времени  Unix/ Linux- системы ) и  применить функцию  ctime(..)  */  
  tmit- = 2208988800U;
  printf("Server Time: %s",(char*)ctime(&tmit));
  i=time(0);
  printf("System Time: %s",(char*)ctime(&i));
  printf("System time is %d seconds off\ n",i- tmit);
  }
