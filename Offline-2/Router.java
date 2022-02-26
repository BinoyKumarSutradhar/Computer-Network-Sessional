//Work needed
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

public class Router {

    private int routerId;
    private int numberOfInterfaces;
    private ArrayList<IPAddress> interfaceAddresses;//list of IP address of all interfaces of the router
    private ArrayList<RoutingTableEntry> routingTable;//used to implement DVR
    private ArrayList<Integer> neighborRouterIDs;//Contains both "UP" and "DOWN" state routers
    private Boolean state;//true represents "UP" state and false is for "DOWN" state
    private Map<Integer, IPAddress> gatewayIDtoIP;
    public Router() {
        interfaceAddresses = new ArrayList<>();
        routingTable = new ArrayList<>();
        neighborRouterIDs = new ArrayList<>();

        /**
         * 80% Probability that the router is up
         */
        Random random = new Random();
        double p = random.nextDouble();
        if(p < 0.80) state = true;
        else state = false;

        numberOfInterfaces = 0;

    }

    public Router(int routerId, ArrayList<Integer> neighborRouters, ArrayList<IPAddress> interfaceAddresses, Map<Integer, IPAddress> gatewayIDtoIP) {
        this.routerId = routerId;
        this.interfaceAddresses = interfaceAddresses;
        this.neighborRouterIDs = neighborRouters;
        this.gatewayIDtoIP = gatewayIDtoIP;
        routingTable = new ArrayList<>();



        /**
         * 80% Probability that the router is up
         */
        Random random = new Random();
        double p = random.nextDouble();
        if(p < 0.80) state = true;
        else state = false;

        numberOfInterfaces = interfaceAddresses.size();
    }

    @Override
    public String toString() {
        String string = "";
        string += "Router ID: " + routerId + "\n" + "Interfaces: \n";
        for (int i = 0; i < numberOfInterfaces; i++) {
            string += interfaceAddresses.get(i).getString() + "\t";
        }
        string += "\n" + "Neighbors: \n";
        for(int i = 0; i < neighborRouterIDs.size(); i++) {
            string += neighborRouterIDs.get(i) + "\t";
        }
        return string;
    }



    /**
     * Initialize the distance(hop count) for each router.
     * for itself, distance=0; for any connected router with state=true, distance=1; otherwise distance=Constants.INFTY;
     */
    public void initiateRoutingTable() {

        ArrayList<Router> duplicate = NetworkLayerServer.routers;
        double distance;

        for (Router rt : duplicate)
        {
            int i = rt.getRouterId();
            RoutingTableEntry entry = null;
            
            if(neighborRouterIDs.contains(i))
            {
                if(rt.getState() == true)
                {
                    distance = 1;
                    entry = new RoutingTableEntry(i,distance,i);
                }
                else
                {
                    distance = Constants.INFINITY;
                    entry = new RoutingTableEntry(i,distance,0);
                }
            }
            else if(i == routerId)
            {
                distance = 0;
                entry = new RoutingTableEntry(i,distance,i);
            }
            else
            {
                distance = Constants.INFINITY;
                entry = new RoutingTableEntry(i,distance,0);
            }

            routingTable.add(entry);
        }
        
    }

    /**
     * Delete all the routingTableEntry
     */
    public void clearRoutingTable() {

        routingTable.clear();

    }

    /**
     * Update the routing table for this router using the entries of Router neighbor
     * @param neighbor
     */
    public boolean updateRoutingTable(Router neighbor) // normal update
    {
        int f = 0;
        double d1 = 0;
        for (RoutingTableEntry rte : routingTable)
        {
            if(rte.getRouterId() == neighbor.getRouterId())
            {
                d1 = rte.getDistance();
            }
        }
        ArrayList<RoutingTableEntry> nbr_rte = neighbor.getRoutingTable();

        for(int i=0; i< routingTable.size(); i++)
        {
            //double x = Constants.INFINITY;
            double c = d1 + nbr_rte.get(i).getDistance();
            if( (routingTable.get(i).getDistance() > c) )
            {
                routingTable.get(i).setDistance(c);
                routingTable.get(i).setGatewayRouterId(neighbor.getRouterId());

                f++;
            }
        }

        if(f >0)
        {
            return true;
        }
        else
        {
            return false;
        }
        //return true;

    }

    public boolean sfupdateRoutingTable(Router neighbor) // Split horizon and force update("sf"update)
    {
        int f = 0;
        int id = 0; // for SPLIT HORIZON
        int my_next_hop = 0;
        double d1 = 0;
        for (RoutingTableEntry rte : routingTable)
        {
            if(rte.getRouterId() == neighbor.getRouterId())
            {
                d1 = rte.getDistance();
            }
        }
        ArrayList<RoutingTableEntry> nbr_rte = neighbor.getRoutingTable();

        for(int i=0; i< routingTable.size(); i++)
        {
            double x = Constants.INFINITY;
            double c = d1 + nbr_rte.get(i).getDistance();
            id = nbr_rte.get(i).getGatewayRouterId();
            my_next_hop = routingTable.get(i).getGatewayRouterId();
            if( (my_next_hop == neighbor.getRouterId()) || ((routingTable.get(i).getDistance() > c) && (id != routerId)) ) //split horizon done
            {
                //aro 1 ta condition check kora lagbe
                if( (routingTable.get(i).getDistance() != c) && (routingTable.get(i).getGatewayRouterId()!=neighbor.getRouterId()) )
                {
                    routingTable.get(i).setDistance(c);
                    routingTable.get(i).setGatewayRouterId(neighbor.getRouterId());
                    f++;
                }

            }
        }

        if(f >0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * If the state was up, down it; if state was down, up it
     */
    public void revertState() {
        state = !state;
        if(state) { initiateRoutingTable(); }
        else { clearRoutingTable(); }
    }

    public int getRouterId() {
        return routerId;
    }

    public void setRouterId(int routerId) {
        this.routerId = routerId;
    }

    public int getNumberOfInterfaces() {
        return numberOfInterfaces;
    }

    public void setNumberOfInterfaces(int numberOfInterfaces) {
        this.numberOfInterfaces = numberOfInterfaces;
    }

    public ArrayList<IPAddress> getInterfaceAddresses() {
        return interfaceAddresses;
    }

    public void setInterfaceAddresses(ArrayList<IPAddress> interfaceAddresses) {
        this.interfaceAddresses = interfaceAddresses;
        numberOfInterfaces = interfaceAddresses.size();
    }

    public ArrayList<RoutingTableEntry> getRoutingTable() {
        return routingTable;
    }

    public void addRoutingTableEntry(RoutingTableEntry entry) {
        this.routingTable.add(entry);
    }

    public ArrayList<Integer> getNeighborRouterIDs() {
        return neighborRouterIDs;
    }

    public void setNeighborRouterIDs(ArrayList<Integer> neighborRouterIDs) { this.neighborRouterIDs = neighborRouterIDs; }

    public Boolean getState() {
        return state;
    }

    public void setState(Boolean state) {
        this.state = state;
    }

    public Map<Integer, IPAddress> getGatewayIDtoIP() { return gatewayIDtoIP; }

    public void printRoutingTable() {
        System.out.println("Router " + routerId);
        System.out.println("DestID Distance Nexthop");
        for (RoutingTableEntry routingTableEntry : routingTable) {
            System.out.println(routingTableEntry.getRouterId() + " " + routingTableEntry.getDistance() + " " + routingTableEntry.getGatewayRouterId());
        }
        System.out.println("-----------------------");
    }
    public String strRoutingTable() {
        String string = "Router" + routerId + "\n";
        string += "DestID Distance Nexthop\n";
        for (RoutingTableEntry routingTableEntry : routingTable) {
            string += routingTableEntry.getRouterId() + " " + routingTableEntry.getDistance() + " " + routingTableEntry.getGatewayRouterId() + "\n";
        }

        string += "-----------------------\n";
        return string;
    }

}
