#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: SLIGHTLY MODIFIED
 FROM VERSION 1.1 of J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
       are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
       or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
       (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 1 /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

//****MINE*****//
#define A 0
#define B 1
//#define t_increment 15.0
#define t_increment 20.0

//****MINE*****//

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct pkt
{
    char data[4];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct frm
{
    int type;
    int seqnum;
    int acknum;
    int checksum;
    char payload[4];
};

/********* FUNCTION PROTOTYPES. DEFINED IN THE LATER PART******************/
int check_sum(struct frm packet); //MINE

void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void tolayer3(int AorB, struct frm packet);
void tolayer5(int AorB, char datasent[4]);

//****MINE****//
int g_msg_pass = 1;
int A_acknum;
int B_acknum;
int A_seqnum;
int B_seqnum;
struct frm packet_A;
struct frm packet_AA;// A_input er jonno
struct frm packet_B;
struct frm packet_BB;// B_output er jonno
struct frm packet_R;
struct frm packet_RA;

int next_packet;
int next_packet_B;
int prev_packet;
int prev_packet_B;
int delivery_update;
int delivery_update_B;
int b_received; // B side e last jeta properly receive korse oitar seq num
int a_received;
int a_flag = 0;
int b_flag = 0;
int match_B = 0;

int p_backing = 0;
int ack_A;
int ack_B;
//****MINE****//

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

int check_sum(struct frm packet)
{
    int x = 0;

    for(int i=0; i<4; i++)
    {
        x = x + packet.payload[i];
    }
    //printf("%d\n",x);
    x = x + packet.seqnum;
    //printf("%d\n",x);
    x = x + packet.acknum;
    //printf("%d\n",x);
    x = x + packet.type;
    //printf("%d\n",x);

    return x;
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct pkt message)
{

    //struct pkt packet_A;
    if(p_backing == 0)
    {
        printf("\n******A_output****\n");
        printf("\n******Data %s waiting to to be sent****\n", message.data);
        if(delivery_update && g_msg_pass)
        {
            delivery_update = 0;  // eta if er vitor hote pare
            memmove(packet_A.payload, message.data, 4);
            packet_A.seqnum = next_packet;
            prev_packet = next_packet;

            packet_A.type = 0;      // EKHANE TYPE ER VALUE add kora hoyeche
            packet_A.checksum = check_sum(packet_A);

            printf("\n----A_output----\n");
            printf("Type--->%d\n",packet_A.type);
            printf("Seq_numb--->%d\n",packet_A.seqnum);
            printf("Ack_numb--->%d\n",packet_A.acknum);
            printf("Checksum--->%d\n",packet_A.checksum);
            printf("Data--->%s\n\n",packet_A.payload);

            //delivery_update = 0; //eta if er shurute hote pare

            g_msg_pass = 0;
            tolayer3(A,packet_A);// send packet

            starttimer(A,t_increment);// start countdown
        }
    }
    if(p_backing == 1)
    {
        printf("\n******A_output****\n");
        printf("\n******Data %s waiting to to be sent****\n", message.data);
        if(delivery_update)
        {
            delivery_update = 0;  // eta if er vitor hote pare
            memmove(packet_A.payload, message.data, 4);
            if(a_flag == 1)
            {
                if(next_packet)
                {
                    next_packet = 0;
                }
                else
                {
                    next_packet = 1;
                }
                ack_A = 0;
                a_flag = 0;
            }
            packet_A.seqnum = next_packet;
            prev_packet = next_packet;

            packet_A.type = 0;      // EKHANE TYPE ER VALUE add kora hoyeche
            //packet_A.checksum = check_sum(packet_A);

            /*printf("\n----A_output----\n");
            printf("Type--->%d\n",packet_A.type);
            printf("Seq_numb--->%d\n",packet_A.seqnum);
            printf("Ack_numb--->%d\n",packet_A.acknum);
            printf("Checksum--->%d\n",packet_A.checksum);
            printf("Data--->%s\n\n",packet_A.payload);*/

            //delivery_update = 0; //eta if er shurute hote pare

            //g_msg_pass = 0;
            if(ack_A == 1)
            {
                packet_A.acknum = a_received;
                packet_A.type = 2;
                packet_A.checksum = check_sum(packet_A);
            }
            else
            {
                packet_A.checksum = check_sum(packet_A);
            }
            printf("\n----A_output----\n");
            printf("Type--->%d\n",packet_A.type);
            printf("Seq_numb--->%d\n",packet_A.seqnum);
            printf("Ack_numb--->%d\n",packet_A.acknum);
            printf("Checksum--->%d\n",packet_A.checksum);
            printf("Data--->%s\n\n",packet_A.payload);

            tolayer3(A,packet_A);// send packet

            starttimer(A,t_increment);// start countdown
    }
    }

}

/* need be completed only for extra credit */
void B_output(struct pkt message)
{
    printf("\n******B_output****\n");
    printf("\n******Data %s waiting to to be sent****\n", message.data);
    if(p_backing == 0)
    {
        if(delivery_update_B )
        {
            delivery_update_B = 0;  // eta if er vitor hote pare
            memmove(packet_BB.payload, message.data, 4);
            packet_BB.seqnum = next_packet_B;
            prev_packet_B = next_packet_B;

            packet_BB.type = 0;      // EKHANE TYPE ER VALUE add kora hoyeche
            packet_BB.checksum = check_sum(packet_BB);

            printf("\n----B_output----\n");
            printf("Type--->%d\n",packet_BB.type);
            printf("Seq_numb--->%d\n",packet_BB.seqnum);
            printf("Ack_numb--->%d\n",packet_BB.acknum);
            printf("Checksum--->%d\n",packet_BB.checksum);
            printf("Data--->%s\n\n",packet_BB.payload);

            //delivery_update = 0; //eta if er shurute hote pare

            g_msg_pass = 0;

            tolayer3(B,packet_BB);// send packet

            starttimer(B,t_increment);// start countdown
        }
    }
    //***
    if(p_backing == 1)
    {
        if(delivery_update_B)
        {
            delivery_update_B = 0;  // eta if er vitor hote pare
            memmove(packet_BB.payload, message.data, 4);
            packet_BB.seqnum = next_packet_B;
            prev_packet_B = next_packet_B;

            packet_BB.type = 0;      // EKHANE TYPE ER VALUE add kora hoyeche
            //packet_BB.checksum = check_sum(packet_BB);

            /*printf("\n----B_output----\n");
            printf("Type--->%d\n",packet_BB.type);
            printf("Seq_numb--->%d\n",packet_BB.seqnum);
            printf("Ack_numb--->%d\n",packet_BB.acknum);
            printf("Checksum--->%d\n",packet_BB.checksum);
            printf("Data--->%s\n\n",packet_BB.payload);*/

            //delivery_update = 0; //eta if er shurute hote pare

            g_msg_pass = 0;

            if(ack_B == 1)
            {
                packet_BB.acknum = b_received;
                packet_BB.type = 2;
                packet_BB.checksum = check_sum(packet_BB);
            }
            else
            {
                packet_BB.checksum = check_sum(packet_BB);
            }
            printf("\n----B_output----\n");
            printf("Type--->%d\n",packet_BB.type);
            printf("Seq_numb--->%d\n",packet_BB.seqnum);
            printf("Ack_numb--->%d\n",packet_BB.acknum);
            printf("Checksum--->%d\n",packet_BB.checksum);
            printf("Data--->%s\n\n",packet_BB.payload);

            tolayer3(B,packet_BB);// send packet

            starttimer(B,t_increment);// start countdown
        }
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct frm packet)
{
    int x = check_sum(packet);

    printf("\n----A_input----\n");
    printf("Type--->%d\n",packet.type);
    printf("Seq_numb--->%d\n",packet.seqnum);
    printf("Ack_numb--->%d\n",packet.acknum);
    printf("Data--->%s\n",packet.payload);
    printf("packet.Checksum--->%d\n",packet.checksum);
    printf("A_side.Checksum--->%d\n\n",x);

    //ekhane kaj kora lagbe.....vul hoise maybe

    if(p_backing == 0)
    {
        if(x == packet.checksum)
        {
            //*****Receiving data frame
            if(packet.type == 0)
            {

            printf("1. Packet %d received at A successfully!!\n",packet.seqnum);

            if(a_received == packet.seqnum)
            {

                packet_AA.type = 1;
                packet_AA.acknum = 0;
                packet_AA.seqnum = -1;
                strcpy(packet_AA.payload,"");
                packet_AA.checksum = check_sum(packet_AA);
                printf("2. Acknowledgement Packet for Duplicate packet %d sent to B !!\n",a_received);

                tolayer3(A, packet_AA);

            }
            else
            {
                //tolayer5(A,packet.payload);
                packet_AA.type = 1;
                packet_AA.acknum = packet.seqnum;
                packet_AA.seqnum = packet.seqnum;
                //b_received = packet.seqnum;
                strcpy(packet_AA.payload,packet.payload);
                packet_AA.checksum = check_sum(packet_AA);

                //FOR RETRANSMISSION
                packet_RA = packet;
                a_received = packet.seqnum;

                printf("2. Acknowledgement Packet %d sent to B !!\n",packet_AA.seqnum);

                tolayer3(A, packet_AA);
            }
            }

            if(packet.type == 1)
            {

            if(next_packet == packet.seqnum)
            {
                printf("3. Acknowledgement Packet %d received at A successfully!!\n",packet.seqnum);

                if(next_packet)
                {
                    next_packet = 0;
                }
                else
                {
                    next_packet = 1;
                }

                stoptimer(A);
                delivery_update = 1;
                g_msg_pass = 1;
                //ack_A = 0;
            }
            else if(packet.seqnum == -1)
            {
                printf("3. Acknowledgement Packet for Duplicate Packet %d received at A successfully!!\n",packet.seqnum);

                // if-else e vul thakte pare
                if(next_packet)
                {
                    next_packet = 0;
                }
                else
                {
                    next_packet = 1;
                }

                stoptimer(A);
                delivery_update = 1;
                g_msg_pass = 1;
                //ack_A = 0; // eta bad jete pare
            }
            else
            {
                printf("\nNegative Acknowledgement Packet Received!!\n");
                printf("Will retransmit in the TIMER_INTERRUPT method after timeout.....\n");

            }
            }
        }
        else
        {
            printf("**A_input**\n");
            if(packet.type == 0)
            {
                printf("\nSending Negative Acknowledgement Packet(NACK) to *B* as Corrupted Packet was received!!!\n");
                packet_RA.type = 1;
                int y = check_sum(packet_RA);
                packet_RA.checksum = y;
                tolayer3(A,packet_RA);
            }

            if(packet.type == 1)
            {
                printf("\nCorrupted Acknowledgement Packet received!!\n");
                printf("Will retransmit in the TIMER_INTERRUPT method after timeout.....\n");
            }

        }
    }
    if(p_backing == 1)
    {
        if(x == packet.checksum)
        {
            //*****Receiving data frame
            if(packet.type == 0)
            {

                printf("1. Packet %d received at A successfully!!\n",packet.seqnum);

                if(a_received == packet.seqnum)
                {

                    packet_AA.type = 1;
                    packet_AA.acknum = 0;
                    packet_AA.seqnum = -1;
                    strcpy(packet_AA.payload,"");
                    packet_AA.checksum = check_sum(packet_AA);
                    printf("2. Acknowledgement Packet for Duplicate packet %d sent to B !!\n",a_received);
                    ack_A = 0;

                    tolayer3(A, packet_AA);

                }
                else
                {
                    //tolayer5(A,packet.payload);
                    packet_AA.type = 1;
                    packet_AA.acknum = packet.seqnum;
                    packet_AA.seqnum = packet.seqnum;
                    //b_received = packet.seqnum;
                    strcpy(packet_AA.payload,packet.payload);
                    packet_AA.checksum = check_sum(packet_AA);

                    //FOR RETRANSMISSION
                    packet_RA = packet;
                    a_received = packet.seqnum;
                    ack_A = 1;

                    printf("2. Waiting to send Acknowledgement Packet %d to B !!\n",packet_AA.seqnum);
                    //tolayer3(A, packet_AA);
                }
            }

            if(packet.type == 1)
            {

                if(next_packet == packet.seqnum)
                {
                    printf("3. Acknowledgement Packet %d received at A successfully!!\n",packet.seqnum);

                    if(next_packet)
                    {
                        next_packet = 0;
                    }
                    else
                    {
                        next_packet = 1;
                    }

                    stoptimer(A);
                    delivery_update = 1;
                    g_msg_pass = 1;
                    ack_A = 0;
                }
                else if(packet.seqnum == -1)
                {
                    printf("3. Acknowledgement Packet for Duplicate Packet %d received at A successfully!!\n",packet.seqnum);

                    // if-else e vul thakte pare
                    if(next_packet)
                    {
                        next_packet = 0;
                    }
                    else
                    {
                        next_packet = 1;
                    }

                    stoptimer(A);
                    delivery_update = 1;
                    g_msg_pass = 1;
                    ack_A = 0;  // eta deya lagte pare
                }
                else
                {
                    printf("\nNegative Acknowledgement Packet Received!!\n");
                    printf("Will retransmit in the TIMER_INTERRUPT method after timeout.....\n");

                }
            }

            if(packet.type == 2)
            {
                if(next_packet == packet.seqnum)
                {
                    printf("3. PiggyBacked Acknowledgement Packet %d received at A successfully!!\n",packet.seqnum);

                    if(next_packet)
                    {
                        next_packet = 0;
                    }
                    else
                    {
                        next_packet = 1;
                    }

                    stoptimer(A);
                    delivery_update = 1;
                    g_msg_pass = 1;
                    ack_A = 1;

                    packet_RA = packet;
                    a_received = packet.seqnum;

                    //**********************
                    /*packet_AA.type = 1;
                    packet_AA.acknum = packet.seqnum;
                    packet_AA.seqnum = packet.seqnum;
                    //b_received = packet.seqnum;
                    strcpy(packet_AA.payload,packet.payload);
                    packet_AA.checksum = check_sum(packet_AA);

                    //FOR RETRANSMISSION
                    packet_RA = packet;
                    a_received = packet.seqnum;
                    ack_A = 1;*/

                    //printf("2. Acknowledgement Packet %d sent to B !!\n",packet_AA.seqnum);
                    //tolayer3(A, packet_AA);
                }
                else
                {
                    printf("3. Duplicate PiggyBackAcknowledgement Packet %d received at A successfully!!\n",packet.seqnum);

                    // if-else e vul thakte pare
                    if(next_packet)
                    {
                        next_packet = 0;
                    }
                    else
                    {
                        next_packet = 1;
                    }

                    //stoptimer(A);
                    delivery_update = 1;
                    g_msg_pass = 1;

                    //*******

                    packet_AA.type = 1;
                    packet_AA.acknum = 0;
                    packet_AA.seqnum = -1;
                    strcpy(packet_AA.payload,"");
                    packet_AA.checksum = check_sum(packet_AA);
                    printf("2. Acknowledgement Packet for Duplicate Piggyback packet %d sent to B !!\n",a_received);
                    ack_A = 0;
                    tolayer3(A, packet_AA);
                }
            }
        }
        else
        {
            printf("**A_input**\n");
            if(packet.type == 0 || packet.type == 2)
            {
                printf("\nSending Negative Acknowledgement Packet(NACK) to *B* as Corrupted Packet was received!!!\n");
                packet_RA.type = 1;
                int y = check_sum(packet_RA);
                packet_RA.checksum = y;
                tolayer3(A,packet_RA);
            }

            if(packet.type == 1)
            {
                printf("\nCorrupted Acknowledgement Packet received!!\n");
                printf("Will retransmit in the TIMER_INTERRUPT method after timeout.....\n");
            }

        }
    }

}

/* called when A's timer goes off */
void A_timerinterrupt(void)
{
    printf("A_timerinterrupt\n");
    printf("\nTime is up....Retransmitting packet in INTERRUPT method!!\n");
    tolayer3(A,packet_A);// send packet

    starttimer(A,t_increment);// start countdown

}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void)
{
    printf("\nInitializing *A*\n");
    packet_A.acknum = -1;
    packet_A.seqnum = -1;
    packet_A.type = -1;
    next_packet = 0;
    prev_packet = 1;
    delivery_update = 1;
    a_received = -1;
    packet_AA.acknum = -1;
    packet_AA.seqnum = -1;
    packet_AA.type = -1;
    ack_A = -1;

    /*packet_RA.acknum = -1;
    packet_RA.seqnum = -1;
    packet_RA.type = 0;
    strcpy(packet_RA.payload,"");*/
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct frm packet)
{
    int x = check_sum(packet);

    printf("\n----B_input----\n");
    printf("Type--->%d\n",packet.type);
    printf("Seq_numb--->%d\n",packet.seqnum);
    printf("Ack_numb--->%d\n",packet.acknum);
    printf("Data--->%s\n",packet.payload);
    printf("packet.Checksum--->%d\n",packet.checksum);
    printf("B_side.Checksum--->%d\n\n",x);


    if(p_backing == 0)
    {

        if(x == packet.checksum)
        {
            //tolayer5(B,packet.payload);

            //*****Receiving data frame
            if(packet.type == 0)
            {

            printf("1. Packet %d received at B successfully!!\n",packet.seqnum);

            if(b_received == packet.seqnum)
            {

                packet_B.type = 1;
                packet_B.acknum = 0;
                packet_B.seqnum = -1;
                strcpy(packet_B.payload,"");
                packet_B.checksum = check_sum(packet_B);
                printf("2. Acknowledgement Packet for Duplicate packet %d sent at A !!\n",b_received);
                //ack_B = 0;

                tolayer3(B, packet_B);

            }
            else
            {
                tolayer5(B,packet.payload);
                packet_B.type = 1;
                packet_B.acknum = packet.seqnum;
                packet_B.seqnum = packet.seqnum;
                //b_received = packet.seqnum;
                strcpy(packet_B.payload,packet.payload);
                packet_B.checksum = check_sum(packet_B);

                //FOR RETRANSMISSION
                packet_R = packet;
                b_received = packet.seqnum;

                printf("2. Acknowledgement Packet %d sent at A !!\n",packet_B.seqnum);

                tolayer3(B, packet_B);
            }
            }

            //******Receiving ACK packet
            if(packet.type == 1)
            {
            if(next_packet_B == packet.seqnum)
            {
                printf("3. Acknowledgement Packet %d received at B successfully!!\n",packet.seqnum);

                if(next_packet_B)
                {
                    next_packet_B = 0;
                }
                else
                {
                    next_packet_B = 1;
                }

                stoptimer(B);
                delivery_update_B = 1;
                g_msg_pass = 1;
            }
            else if(packet.seqnum == -1)
            {
                printf("3. Acknowledgement Packet for Duplicate Packet %d received at B successfully!!\n",packet.seqnum);

                // if-else e vul thakte pare
                if(next_packet_B)
                {
                    next_packet_B = 0;
                }
                else
                {
                    next_packet_B = 1;
                }

                stoptimer(B);
                delivery_update_B = 1;
                g_msg_pass = 1;
            }
            else
            {
                printf("\nNegative Acknowledgement Packet Received at B!!\n");
                printf("Will retransmit in the TIMER_INTERRUPT method after timeout.....\n");

            }
            }

        }

        else
        {
            //printf("Corrupted packet received!!\n");
            //still to be done
            printf("**B_input**\n");
            if(packet.type == 0)
            {
                printf("\nSending Negative Acknowledgement Packet(NACK) to *A* as Corrupted Packet was received!!!\n");
                packet_R.type = 1;
                int y = check_sum(packet_R);
                packet_R.checksum = y;
                tolayer3(B,packet_R);
            }

            if(packet.type == 1)
            {
                printf("\nCorrupted Acknowledgement Packet received!!\n");
                printf("Will retransmit in the TIMER_INTERRUPT method after timeout.....\n");
            }
        }
    }
    //*******
    if(p_backing == 1)
    {

        if(x == packet.checksum)
        {
            //tolayer5(B,packet.payload);

            //*****Receiving data frame
            if(packet.type == 0)
            {

            printf("1. Packet %d received at B successfully!!\n",packet.seqnum);

            if(b_received == packet.seqnum)
            {

                packet_B.type = 1;
                packet_B.acknum = 0;
                packet_B.seqnum = -1;
                strcpy(packet_B.payload,"");
                packet_B.checksum = check_sum(packet_B);
                printf("2. Acknowledgement Packet for Duplicate packet %d sent at A !!\n",b_received);
                ack_B = 0;
                tolayer3(B, packet_B);

            }
            else
            {
                tolayer5(B,packet.payload);
                packet_B.type = 1;
                packet_B.acknum = packet.seqnum;
                packet_B.seqnum = packet.seqnum;
                //b_received = packet.seqnum;
                strcpy(packet_B.payload,packet.payload);
                packet_B.checksum = check_sum(packet_B);

                //FOR RETRANSMISSION
                packet_R = packet;
                b_received = packet.seqnum;

                printf("2.Waiting to send Acknowledgement Packet %d at A !!\n",packet_B.seqnum);
                ack_B = 1;

                //tolayer3(B, packet_B);
            }
            }

            //******Receiving ACK packet
            if(packet.type == 1)
            {
                if(next_packet_B == packet.seqnum)
                {
                    printf("3. Acknowledgement Packet %d received at B successfully!!\n",packet.seqnum);

                    if(next_packet_B)
                    {
                        next_packet_B = 0;
                    }
                    else
                    {
                        next_packet_B = 1;
                    }
                    a_flag = 1;
                    stoptimer(B);
                    delivery_update_B = 1;
                    g_msg_pass = 1;
                    ack_B = 0;
                }
                else if(packet.seqnum == -1)
                {
                    printf("3. Acknowledgement Packet for Duplicate Packet %d received at B successfully!!\n",packet.seqnum);

                    // if-else e vul thakte pare
                    if(next_packet_B)
                    {
                        next_packet_B = 0;
                    }
                    else
                    {
                        next_packet_B = 1;
                    }

                    stoptimer(B);
                    delivery_update_B = 1;
                    g_msg_pass = 1;
                    ack_B = 0;
                }
                else
                {
                    printf("\nNegative Acknowledgement Packet Received at B!!\n");
                    printf("Will retransmit in the TIMER_INTERRUPT method after timeout.....\n");

                }
            }
            //****
            if(packet.type == 2)
            {
                if(next_packet_B == packet.seqnum)
                {
                    printf("3. PiggyBacked Acknowledgement Packet %d received at B successfully!!\n",packet.seqnum);

                    if(next_packet_B)
                    {
                        next_packet_B = 0;
                    }
                    else
                    {
                        next_packet_B = 1;
                    }
                    match_B = 1;
                    stoptimer(B);
                    delivery_update_B = 1;
                    g_msg_pass = 1;
                    ack_B = 1;

                    packet_R = packet;
                    b_received = packet.seqnum;
                    printf("Waiting to send acknowledgement packet at A_input.....\n");

                    //**********************
                    /*packet_AA.type = 1;
                    packet_AA.acknum = packet.seqnum;
                    packet_AA.seqnum = packet.seqnum;
                    //b_received = packet.seqnum;
                    strcpy(packet_AA.payload,packet.payload);
                    packet_AA.checksum = check_sum(packet_AA);

                    //FOR RETRANSMISSION
                    packet_RA = packet;
                    a_received = packet.seqnum;
                    ack_A = 1;*/

                    //printf("2. Acknowledgement Packet %d sent to B !!\n",packet_AA.seqnum);
                    //tolayer3(A, packet_AA);
                }
                else
                {
                    printf("3. Duplicate PiggyBackAcknowledgement Packet %d received at B successfully!!\n",packet.seqnum);

                    // if-else e vul thakte pare
                    if(match_B != 1)
                    {
                        if(next_packet_B)
                        {
                            next_packet_B = 0;
                        }
                        else
                        {
                            next_packet_B = 1;
                        }
                        match_B = 0;
                    }


                    //stoptimer(B);
                    delivery_update_B = 1;
                    g_msg_pass = 1;

                    //*******

                    packet_B.type = 1;
                    packet_B.acknum = 0;
                    packet_B.seqnum = -1;
                    strcpy(packet_B.payload,"");
                    packet_B.checksum = check_sum(packet_B);
                    printf("2. Acknowledgement Packet for Duplicate Piggyback packet %d sent to A !!\n",b_received);
                    ack_B = 0;
                    tolayer3(B, packet_B);
                }
            }

        }

        else
        {
            //printf("Corrupted packet received!!\n");
            //still to be done
            printf("**B_input**\n");
            if(packet.type == 0 || packet.type == 2)
            {
                printf("\nSending Negative Acknowledgement Packet(NACK) to *A* as Corrupted Packet was received!!!\n");
                packet_R.type = 1;
                int y = check_sum(packet_R);
                packet_R.checksum = y;
                tolayer3(B,packet_R);
            }

            if(packet.type == 1)
            {
                printf("\nCorrupted Acknowledgement Packet received!!\n");
                printf("Will retransmit in the TIMER_INTERRUPT method after timeout.....\n");
            }
        }
    }

}

/* called when B's timer goes off */
void B_timerinterrupt(void)
{
    //printf("  B_timerinterrupt: B doesn't have a timer. ignore.\n");
    printf("B_timerinterrupt\n");
    printf("\nTime is up....Retransmitting packet in INTERRUPT method!!\n");
    //printf("\n----A_input----\n");
    printf("Type--->%d\n",packet_BB.type);
    printf("Seq_numb--->%d\n",packet_BB.seqnum);
    printf("Ack_numb--->%d\n",packet_BB.acknum);
    printf("Data--->%s\n",packet_BB.payload);
    printf("packet.Checksum--->%d\n",packet_BB.checksum);
    //printf("A_side.Checksum--->%d\n\n",x);
    tolayer3(B,packet_BB);// send packet

    starttimer(B,t_increment);
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void)
{
    printf("\nInitializing *B*\n");
    packet_B.acknum = -1;
    packet_B.seqnum = -1;
    b_received = -1;    // duplicate packet receive korar jonno
    packet_B.type = -1;
    delivery_update_B = 1;
    next_packet_B = 0;
    prev_packet_B = 1;
    packet_BB.acknum = -1;
    packet_BB.seqnum = -1;
    //b_received = -1;    // duplicate packet receive korar jonno
    packet_BB.type = -1;
    ack_B = -1;
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
    - emulates the tranmission and delivery (possibly with bit-level corruption
        and packet loss) of packets across the layer 3/4 interface
    - handles the starting/stopping of a timer, and generates timer
        interrupts (resulting in calling students timer handler).
    - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event
{
    float evtime;       /* event time */
    int evtype;         /* event type code */
    int eventity;       /* entity where event occurs */
    struct frm *pktptr; /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};
struct event *evlist = NULL; /* the event list */

/* possible events: */
#define TIMER_INTERRUPT 0
#define FROM_LAYER5 1
#define FROM_LAYER3 2

#define OFF 0
#define ON 1
#define A 0
#define B 1

int TRACE = 1;     /* for my debugging */
int nsim = 0;      /* number of messages from 5 to 4 so far */
int nsimmax = 0;   /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;    /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda;      /* arrival rate of messages from layer 5 */
int ntolayer3;     /* number sent into layer 3 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

void init();
void generate_next_arrival(void);
void insertevent(struct event *p);

int main()
{
    struct event *eventptr;
    struct pkt msg2give;
    struct frm pkt2give;

    int i, j;
    char c;

    init();
    A_init();
    B_init();

    while (1)
    {
        eventptr = evlist; /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next; /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2)
        {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer5 ");
            else
                printf(", fromlayer3 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime; /* update time to next event time */
        if (eventptr->evtype == FROM_LAYER5)
        {
            if (nsim < nsimmax)
            {
                if (nsim + 1 < nsimmax)
                    generate_next_arrival(); /* set up future arrival */
                /* fill in msg to give with string of same letter */
                j = nsim % 26;
                for (i = 0; i < 4; i++)
                    msg2give.data[i] = 97 + j;
                msg2give.data[3] = 0;
                if (TRACE > 2)
                {
                    printf("          MAINLOOP: data given to student: ");
                    for (i = 0; i < 4; i++)   // ekhane i<4 deya lagte pare
                        printf("%c", msg2give.data[i]);
                    printf("\n");
                }
                nsim++;
                if (eventptr->eventity == A)
                    A_output(msg2give);
                else
                    B_output(msg2give);
            }
        }
        else if (eventptr->evtype == FROM_LAYER3)
        {
            pkt2give.type = eventptr->pktptr->type;
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i = 0; i < 4; i++)            // ekhane 4 bar loop chalaite hobe
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
            if (eventptr->eventity == A) /* deliver packet by calling */
                A_input(pkt2give); /* appropriate entity */
            else
                B_input(pkt2give);
            free(eventptr->pktptr); /* free the memory for packet */
        }
        else if (eventptr->evtype == TIMER_INTERRUPT)
        {
            if (eventptr->eventity == A)
                A_timerinterrupt();
            else
                B_timerinterrupt();
        }
        else
        {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

terminate:
    printf(
        " Simulator terminated at time %f\n after sending %d msgs from layer5\n",
        time, nsim);
}

void init() /* initialize the simulator */
{
    int i;
    float sum, avg;
    float jimsrand();

    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of messages to simulate: ");
    scanf("%d",&nsimmax);
    printf("Enter PiggyBacking: ");
    scanf("%d",&p_backing);
    printf("Enter  packet loss probability [enter 0.0 for no loss]:");
    scanf("%f",&lossprob);
    printf("Enter packet corruption probability [0.0 for no corruption]:");
    scanf("%f",&corruptprob);
    printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
    scanf("%f",&lambda);
    printf("Enter TRACE:");
    scanf("%d",&TRACE);

    srand(9999); /* init random number generator */
    sum = 0.0;   /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75)
    {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(1);
    }

    ntolayer3 = 0;
    nlost = 0;
    ncorrupt = 0;

    time = 0.0;              /* initialize time to 0.0 */
    generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand(void)
{
    double mmm = RAND_MAX;
    float x;                 /* individual students may need to change mmm */
    x = rand() / mmm;        /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival(void)
{
    double x, log(), ceil();
    struct event *evptr;
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + x;
    evptr->evtype = FROM_LAYER5;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(struct event *p)
{
    struct event *q, *qold;

    if (TRACE > 2)
    {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist;      /* q points to header of list in which p struct inserted */
    if (q == NULL)   /* list is empty */
    {
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else
    {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL)   /* end of list */
        {
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist)     /* front of list */
        {
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else     /* middle of list */
        {
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

void printevlist(void)
{
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next)
    {
        printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
               q->eventity);
    }
    printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(int AorB /* A or B is trying to stop timer */)
{
    struct event *q, *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f\n", time);
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;          /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist)   /* front of list - there must be event after */
            {
                q->next->prev = NULL;
                evlist = q->next;
            }
            else     /* middle of list */
            {
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(int AorB /* A or B is trying to start timer */, float increment)
{
    struct event *q;
    struct event *evptr;

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f\n", time);
    /* be nice: check to see if timer is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }

    /* create future event for when timer goes off */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);
}

/************************** TOLAYER3 ***************/
void tolayer3(int AorB, struct frm packet)
{
    struct frm *mypktptr;
    struct event *evptr, *q;
    float lastime, x;
    int i;

    ntolayer3++;

    /* simulate losses: */
    if (jimsrand() < lossprob)
    {
        nlost++;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being lost\n");
        return;
    }

    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    mypktptr = (struct frm *)malloc(sizeof(struct frm));
    mypktptr->type = packet.type;
    mypktptr->seqnum = packet.seqnum;
    mypktptr->acknum = packet.acknum;
    mypktptr->checksum = packet.checksum;
    for (i = 0; i < 4; i++)        // EKHANE 4 bar loop chaliate hobe
        mypktptr->payload[i] = packet.payload[i];
    if (TRACE > 2)
    {
        printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
               mypktptr->acknum, mypktptr->checksum);
        for (i = 0; i < 4; i++)    // Ekhane  4 bar loop chaliate hobe
            printf("%c", mypktptr->payload[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtype = FROM_LAYER3;      /* packet will pop out from layer3 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->pktptr = mypktptr;         /* save ptr to my copy of packet */
    /* finally, compute the arrival time of packet at the other end.
       medium can not reorder, so make sure packet arrives between 1 and 10
       time units after the latest arrival time of packets
       currently in the medium on their way to the destination */
    lastime = time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corruptprob)
    {
        ncorrupt++;
        if ((x = jimsrand()) < .75)
            mypktptr->payload[0] = 'Z'; /* corrupt payload */
        else if (x < .875)
            mypktptr->seqnum = 999999;
        else
            mypktptr->acknum = 999999;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TOLAYER3: scheduling arrival on other side\n");
    insertevent(evptr);
}

void tolayer5(int AorB, char datasent[4])
{
    int i;
    if (TRACE > 2)
    {
        printf("          TOLAYER5: data received: ");
        for (i = 0; i < 4; i++)    // ekhane  4 bar loop chaliate hobe
            printf("%c", datasent[i]);
        printf("\n");
    }
}

