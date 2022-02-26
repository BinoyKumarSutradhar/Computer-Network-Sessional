import java.util.ArrayList;
import java.util.Random;

//Work needed
public class Client {
    public static void main(String[] args) throws InterruptedException {
        NetworkUtility networkUtility = new NetworkUtility("127.0.0.1", 4444);
        System.out.println("Connected to server");
        EndDevice rcvd = (EndDevice) networkUtility.read();
        String x = String.valueOf(networkUtility.read());
        String[] arrOfStr = x.split("\n");
        for (String a : arrOfStr)
            System.out.println(a);

        //System.out.println(randomNum);
        //System.out.println(a);

        for(int i=0;i<100;i++)
        {
            String y = msg();
            boolean z = true;
            IPAddress src_ip = null;
            IPAddress dst_ip = null;



                Random rand = new Random();
                int randomNum = rand.nextInt(arrOfStr.length);
                String a = arrOfStr[randomNum];

                src_ip = rcvd.getIpAddress();
                dst_ip = new IPAddress(a);


            // ekhane problem ase

            if(i == 20)
            {
                Packet pc = new Packet(y,"SHOW_ROUTE",src_ip,dst_ip);
                networkUtility.write(pc);
            }
            else
            {
                Packet pc = new Packet(y,"",src_ip,dst_ip);
                //System.out.println(pc);
                //Packet pc = new Packet(y,"",src_ip,new IPAddress(a));
                networkUtility.write(pc);
            }

        }

        //System.out.println(x);
        /**
         * Tasks
         */
        
        /*
        1. Receive EndDevice configuration from server
        2. Receive active client list from server
        3. for(int i=0;i<100;i++)
        4. {
        5.      Generate a random message
        6.      Assign a random receiver from active client list
        7.      if(i==20)
        8.      {
        9.            Send the message and recipient IP address to server and a special request "SHOW_ROUTE"
        10.           Display routing path, hop count and routing table of each router [You need to receive
                            all the required info from the server in response to "SHOW_ROUTE" request]
        11.     }
        12.     else
        13.     {
        14.           Simply send the message and recipient IP address to server.
        15.     }
        16.     If server can successfully send the message, client will get an acknowledgement along with hop count
                    Otherwise, client will get a failure message [dropped packet]
        17. }
        18. Report average number of hops and drop rate
        */
    }

    public static String msg()
    {
        int leftLimit = 97; // letter 'a'
        int rightLimit = 122; // letter 'z'
        int targetStringLength = 10;
        Random random = new Random();
        StringBuilder buffer = new StringBuilder(targetStringLength);
        for (int i = 0; i < targetStringLength; i++) {
            int randomLimitedInt = leftLimit + (int)
                    (random.nextFloat() * (rightLimit - leftLimit + 1));
            buffer.append((char) randomLimitedInt);
        }
        String generatedString = buffer.toString();

        //System.out.println(generatedString);
        return generatedString;
    }
}
