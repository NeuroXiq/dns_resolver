int print_header(DNS_MESSAGE *msg)
{
	printf("ID\t: %X%X\n", (unsigned char)msg->Header.ID[0], (unsigned char)msg->Header.ID[1]);
	printf("QR\t: %X\n", msg->Header.QR);
	printf("Opcode\t: %X\n", msg->Header.Opcode);
	printf("AA\t: %X\n", msg->Header.AA);
	printf("TC\t: %X\n", msg->Header.TC);
	printf("RD\t: %X\n", msg->Header.RD);
	printf("RA\t: %X\n", msg->Header.RA);
	printf("Z\t: %X\n", msg->Header.Z);
	printf("RCODE\t: %X\n", msg->Header.RCODE);
	
	printf("QDCOUNT\t: %X\n", msg->Header.QDCOUNT);
	printf("ANCOUNT\t: %X\n", msg->Header.ANCOUNT);
	printf("NSCOUNT\t: %X\n", msg->Header.NSCOUNT);
	printf("ARCOUNT\t: %X\n", msg->Header.ARCOUNT);
	
	return 0;
}

int print_questions(DNS_MESSAGE *msg)
{
	for(int i = 0; i < msg->Header.QDCOUNT; i++)
	{
		printf("[QUESTION %d]\n",i);
		printf("QNAME : %s\n", msg->Question[i].QNAME);
		printf("QTYPE : %.4X\n",msg->Question[i].QTYPE);
		printf("QCLASS: %.4X\n",msg->Question[i].QCLASS);
	}
	return 0;
}

int print_resources(DNS_MESSAGE_RESOURCE *res, int count, char *res_name)
{
	for(int i = 0 ; i < count; i++)
	{
		printf("[RESOURCE %s Count index: %d]\n", res_name, i);
		printf("NAME  : %s\n", res[i].NAME);
		printf("TYPE  : %d\n", res[i].TYPE);
		printf("CLASS : %d\n", res[i].CLASS);
		printf("TTL   : %d\n", res[i].TTL);
		printf("RDLENGTH   : %d\n", res[i].RDLENGTH);
		printf("RDATA :\n");
		printf("---- RDATA START ----\n");
		for(int j = 0; j < res[i].RDLENGTH; j++)
		{
			
			printf(" %X",(unsigned char)res[i].RDATA[j] );
			if(((i+1)%16 )== 0)
			{
				printf("\n");
			}
		}
		printf("\n---- DATA END --	--\n");

	}

}