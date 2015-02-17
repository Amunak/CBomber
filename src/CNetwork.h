#ifndef CNETWORK_H
#define	CNETWORK_H

#include <exception>

/**
 * Exception thrown on network errors
 */
class CNetworkException : public std::exception {
public:
    /**
     * Describes this exception
     * @see std::exception::what
     */
    virtual const char* what() const noexcept;
};

class CNetwork {
public:
    /**
     * A port this game uses for its connections
     */
    const static int PORT = 24042;

    /**
     * Size of the buffer used to transfer all game data to client
     */
    const static int BUFFER_SIZE = 1222;

    /**
     * Prepares the server listener
     * @param name server name/address
     */
    static void InitServer(const char * name);

    /**
     * Prepares the client socket
     * @param name server name/address
     */
    static void InitClient(const char * name);

    /**
     * Closes all sockets
     */
    static void CloseConnection();

    /**
     * Processes data from client, sends back screen
     */
    static void ServeClient();

    /**
     * Sends what we did to server, loads its processed world 
     * @param action the action client did
     * @return true on game end
     */
    static bool SendAction(const char action);

private:
    /**
     * Saves the FD of current connection
     */
    static int m_fileDescriptor;

    /**
     * Saves the FD of data connection
     */
    static int m_dataFileDescriptor;
    
    /**
     * Holds amount of skipped data transfers
     */
    static int m_skippedDT;

    /**
     * This is a static class. Prevents instantiation
     */
    CNetwork();

    /**
     * Opens a listening socket on specified port and address with family name
     * @author Ladislav Vagner
     * @param name protocol-address field
     * @param port desired port
     * @return file descriptor ID
     */
    static int openSrvSocket(const char * name, int port);

    /**
     * Opens a listening socket on specified port and address with family name
     * @author Ladislav Vagner
     * @param name protocol-address field
     * @param port desired port
     * @return file descriptor ID
     */
    static int openCliSocket(const char * name, int port);
};

#endif	/* CNETWORK_H */

