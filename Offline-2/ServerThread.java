

import java.util.ArrayList;
import java.util.Map;
import java.util.Random;

public class ServerThread implements Runnable {

    NetworkUtility networkUtility;
    EndDevice endDevice;

    ServerThread(NetworkUtility networkUtility, EndDevice endDevice) {
        this.networkUtility = networkUtility;
        this.endDevice = endDevice;
        System.out.println("Server Ready for client " + NetworkLayerServer.clientCount);
        //NetworkLayerServer.clientCount++;
        new Thread(this).start();
    }

    @Override
    public void run() {
        /**
         * Synchronize actions with client.
         */
        
        /*
        Tasks:
        1. Upon receiving a packet and recipient, call deliverPacket(packet)
        2. If the packet contains "SHOW_ROUTE" request, then fetch the required information
                and send back to client
        3. Either send acknowledgement with number of hops or send failure message back to client
        */

        ArrayList<EndDevice> ed = NetworkLayerServer.endDevices;
        String x = "";
        for(EndDevice e: ed)
        {
            x = x.concat(String.valueOf(e.getIpAddress())+"\n");
        }
        //System.out.println(x);
        networkUtility.write(endDevice);
        networkUtility.write(x);
        int g =0;
        int h=0;

        for(int i=0;i<100;i++)
        {
            Packet p = (Packet)networkUtility.read();


            if(NetworkLayerServer.endDevices.size() > 1)
            {
                boolean f = true;
                f = deliverPacket(p);
                if(f)
                {
                    g++;
                }
                else
                {
                    h++;
                }
            }
        }

        System.out.println("Packet delivered: "+g);
        System.out.println("Packet dropped: "+h);

    }


    public Boolean deliverPacket(Packet p) {


        //System.out.println(p);

        //router id ber korbo hashmap diye
        //router ber korbo
        Router S = null;
        Router D = null;
        Map<IPAddress, Integer> rt_id = NetworkLayerServer.interfacetoRouterID;
        Map<Integer, Router> find_rt = NetworkLayerServer.routerMap;

        Map<IPAddress, EndDevice> end = NetworkLayerServer.endDeviceMap;
        ArrayList<Router> find = NetworkLayerServer.routers;
        int src_rt=0;
        int dst_rt=0;
        int s_rt = 0;
        int d_rt = 0;
        int cnt = 0;

        IPAddress src = p.getSourceIP();
        String s = String.valueOf(src);
        String[] s_arr = s.split("\\.");

        IPAddress dst = p.getDestinationIP();
        String d = String.valueOf(dst);
        String[] d_arr = d.split("\\.");

        //System.out.println(s);
        //System.out.println(d);

        /*EndDevice e1 = end.get(src);
        IPAddress gate_s = e1.getGateway();
        EndDevice e2 = end.get(dst);
        IPAddress gate_d = e2.getGateway();
        s_rt = rt_id.get(gate_s);
        d_rt = rt_id.get(gate_d);
        Router S1 = find_rt.get(s_rt);
        Router D1 = find_rt.get(d_rt);*/


        for (Map.Entry<IPAddress, Integer> e : rt_id.entrySet())
        {
            IPAddress t = e.getKey();
            String a = String.valueOf(t);
            //System.out.println(a);
            String[] arrOfStr = a.split("\\.");
            for (int i = 0; i < 3; i++) {
                if(!(arrOfStr[i].equalsIgnoreCase(s_arr[i])))
                {
                    break;
                }
                if(i==2 && arrOfStr[i].equalsIgnoreCase(s_arr[i]))
                {
                    Integer m = e.getValue();
                    src_rt = m.intValue();
                    //System.out.println(src_rt);
                    S = find_rt.get(src_rt);
                    //System.out.println("Source: "+S.getRouterId());
                    cnt++;
                }
            }
            for (int i = 0; i < 3; i++) {
                if(!(arrOfStr[i].equalsIgnoreCase(d_arr[i])))
                {
                    break;
                }
                if(i==2 && arrOfStr[i].equalsIgnoreCase(d_arr[i]))
                {
                    Integer n = e.getValue();
                    dst_rt = n.intValue();
                    D = find_rt.get(dst_rt);
                    //System.out.println("Destination: "+D.getRouterId());
                    cnt++;
                }
            }

            if(cnt==2)
            {
                break;
            }

        }

        Router temp = S;
        int b = 0 ;
        int c = 0;
        while(true)
        {
            int tmp_id ;
            int br = 0;
            //int hop_cnt = 0;
            ArrayList<RoutingTableEntry> loop = temp.getRoutingTable();
            for (RoutingTableEntry y: loop)
            {
                if( (y.getRouterId() == D.getRouterId()) && (y.getDistance() != Constants.INFINITY) )
                {
                    if(y.getDistance() == 0)
                    {
                        //System.out.println("Packet delivered!!!");
                        b++;
                        br++;
                        break;
                    }
                    else
                    {
                        tmp_id = y.getGatewayRouterId();
                        if(tmp_id == 0)
                        {
                            c++;
                            br++;
                            break;
                        }
                        Router temp_b = temp;
                        temp = find_rt.get(tmp_id);
                        if(temp.getState() == false)
                        {
                            c++;
                            br++;
                            y.setDistance(Constants.INFINITY);
                            RouterStateChanger.islocked = true;
                            NetworkLayerServer.DVR(temp_b.getRouterId());
                            RouterStateChanger.islocked = false;
                            synchronized (RouterStateChanger.msg)
                            {
                                RouterStateChanger.msg.notify();
                            }
                            break;
                        }
                        for(RoutingTableEntry now : temp.getRoutingTable() )
                        {
                            if(now.getRouterId() == temp_b.getRouterId())
                            {
                                if(now.getDistance() == Constants.INFINITY)
                                {
                                    now.setDistance(1);
                                    RouterStateChanger.islocked = true;
                                    NetworkLayerServer.DVR(temp.getRouterId());
                                    RouterStateChanger.islocked = false;
                                    synchronized (RouterStateChanger.msg)
                                    {
                                        RouterStateChanger.msg.notify();
                                    }

                                }
                            }
                        }

                    }
                }
                else if( (y.getRouterId() == D.getRouterId()) && (y.getDistance() == Constants.INFINITY) )
                {
                    //System.out.println("Packet dropped!!!");
                    c++;
                    br++;
                    break;
                }
                else
                {
                    //c++;
                    br++;
                    break;
                }
                /*if( (y.getRouterId() == D.getRouterId()) && (y.getDistance() == 0) )
                {
                    System.out.println("Packet delivered!!!");
                    br++;
                    break;
                }*/
            }
            if(br!=0)
            {
                break;
            }
        }

        /*System.out.println("Packet delivered: "+b);
        System.out.println("Packet dropped: "+c);*/
        if(b>0)
        {
            return true;
        }
        if(c>0)
        {
            return false;
        }

            //System.out.println("here");
        /*
        1. Find the router s which has an interface
                such that the interface and source end device have same network address.
        2. Find the router d which has an interface
                such that the interface and destination end device have same network address.
        3. Implement forwarding, i.e., s forwards to its gateway router x considering d as the destination.
                similarly, x forwards to the next gateway router y considering d as the destination,
                and eventually the packet reaches to destination router d.

            3(a) If, while forwarding, any gateway x, found from routingTable of router r is in down state[x.state==FALSE]
                    (i) Drop packet
                    (ii) Update the entry with distance Constants.INFTY
                    (iii) Block NetworkLayerServer.stateChanger.t
                    (iv) Apply DVR starting from router r.
                    (v) Resume NetworkLayerServer.stateChanger.t

            3(b) If, while forwarding, a router x receives the packet from router y,
                    but routingTableEntry shows Constants.INFTY distance from x to y,
                    (i) Update the entry with distance 1
                    (ii) Block NetworkLayerServer.stateChanger.t
                    (iii) Apply DVR starting from router x.
                    (iv) Resume NetworkLayerServer.stateChanger.t

        4. If 3(a) occurs at any stage, packet will be dropped,
            otherwise successfully sent to the destination router
        */

        return true;

    }

    @Override
    public boolean equals(Object obj) {
        return super.equals(obj); //To change body of generated methods, choose Tools | Templates.
    }
}
