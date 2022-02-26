import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.StringTokenizer;

public class HTTPServerSkeleton {
    static final int PORT = 6789;

    public static String readFileData(File file, int fileLength) throws IOException {
        FileInputStream fileIn = null;
        byte[] fileData = new byte[fileLength];

        try {
            fileIn = new FileInputStream(file);
            fileIn.read(fileData);
        } finally {
            if (fileIn != null)
                fileIn.close();
        }

        return String.valueOf(fileData);
    }


    static void HTML_generator(File y) throws IOException {

        ArrayList<String> a = new ArrayList<String>(Arrays.asList(y.list()));

        File latest = new File(y.getCanonicalPath() + "\\index.html");

        try (BufferedWriter bf = new BufferedWriter(new FileWriter(latest))) {
            bf.write("<html>\n<body>\n<h2>Path=>" + y.getCanonicalPath() + ":</h2>\n<ol>\n");

            int m;
            m=0;
            while(m<a.size())
            {
                bf.write("<a href=http://localhost:6789/" + y.getCanonicalPath() + "\\" + a.get(m) + "> " + a.get(m) + " </a> " + "<br>\n");
                m++;
            }

            bf.write("</ol>\n</body>\n</html>");
        }
    }

    public static void main(String[] args) throws IOException {

        ServerSocket serverConnect = new ServerSocket(PORT);
        System.out.println("Server started.\nListening for connections on port : " + PORT + " ...\n");


        while (true) {
            Socket s = serverConnect.accept();
            String s1 = " ";
            String s2 = " ";
            BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
            PrintWriter pr = new PrintWriter(s.getOutputStream());
            String input = in.readLine();

            System.out.println("New Client: " + s);

            if (input != null) {

                StringTokenizer tk = new StringTokenizer(input);

                s1 = tk.nextToken();
                //System.out.println(s1);
                s2 = tk.nextToken();
                //System.out.println(s2);

                File f = new File(s2);

                if (s1.equalsIgnoreCase("GET")) {


                    if (f.isFile()) {

                        byte[] b = new byte[(int) f.length()];

                        BufferedInputStream bs = new BufferedInputStream(new FileInputStream(f));
                        OutputStream os = s.getOutputStream();

                        pr.write("HTTP/1.1 200 OK\r\n");
                        pr.write("Server: Java HTTP Server: 1.0\r\n");
                        pr.write("Date: " + new Date() + "\r\n");
                        pr.write("Content-Type: application/x-forcedownload\r\n");
                        pr.write("Content-Length: " + f.length() + "\r\n");
                        pr.write("\r\n");

                        pr.flush();

                        int x = bs.read(b);
                        while (x>0) {
                            //System.out.println(contentLine);
                            os.write(b, 0, x);

                            os.flush();
                            x = bs.read(b);
                        }


                    } else if (f.isDirectory()) {

                        File n1 = new File(s2);

                        if (input == null)
                        {
                            continue;
                        }
                        if (s1.equalsIgnoreCase("GET")) {
                            HTML_generator(n1);
                            File index = new File(s2 + "\\index.html");
                            FileInputStream inst = new FileInputStream(index);
                            BufferedReader br = new BufferedReader(new InputStreamReader(inst, "UTF-8"));

                            StringBuilder ff = new StringBuilder();


                            String rcv = br.readLine();
                            while (rcv != null) {
                                //System.out.println(contentLine);
                                ff.append(rcv);
                                ff.append('\n');
                                rcv = br.readLine();
                            }


                            pr.write("HTTP/1.1 200 OK\r\n");
                            pr.write("Server: Java HTTP Server: 1.0\r\n");
                            pr.write("Date: " + new Date() + "\r\n");
                            pr.write("Content-Type: text/html\r\n");
                            pr.write("Content-Length: " + ff.toString().length() + "\r\n");
                            pr.write("\r\n");

                            pr.write(ff.toString());

                            pr.flush();

                        }

                    }

                    if (!(f.exists())) {
                        String show = "<html>\t<body>\n404(NOT FOUND) \n</body>\t</html>";
                        pr.write("HTTP/1.1 200 OK\r\n");
                        pr.write("Server: Java HTTP Server: 1.0\r\n");
                        pr.write("Date: " + new Date() + "\r\n");
                        pr.write("Content-Type: text/html\r\n");
                        pr.write("Content-Length: " + show.length() + "\r\n");
                        pr.write("\r\n");
                        pr.write(show);
                        pr.flush();

                    }

                }

                //s.close();
            } else {

            }

            s.close();
        }


    }
}
