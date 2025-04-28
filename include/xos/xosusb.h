// The followig defines items related to the XOS USB API

// Define class specific USB IO parameters (all parameters are settable only)

#define IOPAR_USB_ADDRESS   0x8001	// Specify address
#define IOPAR_USB_ENDPNT    0x8002	// Specify endpoint
#define IOPAR_USB_PKTSIZE   0x8003	// Packet size
#define IOPAR_USB_XFERTYPE  0x8004	// Transfer type
#define IOPAR_USB_XFERSPEED 0x8005	// Transfer speed
#define IOPAR_USB_SETUP     0x8006	// Setup token contents
#define IOPAR_USB_PKTRATE   0x8007	// Packet rate
#define IOPAR_USB_INTLIMIT  0x8008	// Interrupt buffer limit

// Define buffer format for the IOPAR_USB_SETUP IO parameter

typedef _Packed struct
{	uchar  reqtype;
	uchar  request;
	ushort value;
	ushort index;
	ushort length;
} SETUP;

// Define values for the IOPAR_USB_XFERTYPE IO parameter

#define USB_XFERTYPE_CNTRL 1	// Control transfer
#define USB_XFERTYPE_INT   2	// Interrupt transfer
#define USB_XFERTYPE_BULK  3	// Bulk transfer
#define USB_XFERTYPE_ISOCH 4	// Isochronous transfer (not supported yet)

#define USB_XFERSPEED_LOW  1	// Low speed transfer
#define USB_XFERSPEED_FULL 2	// Full speed transfer
#define USB_XFERSPEED_HIGH 3	// High speed transfer (not supported yet)

// Define USB special device functions

#define USB_SDF_START       0	// Start controller
#define USB_SDF_STOP        1	// Stop controller
#define USB_SDF_STATUS      2	// Request status
#define USB_SDF_RESETPORT   3	// Reset port
#define USB_SDF_ENABLEPORT  4	// Enable port
#define USB_SDF_DISABLEPORT 5	// Disable port
#define USB_SDF_DISCONNECT  6	// Disconnect client

// Following defines items from the USB standard

// Define device classes

#define CLASS_AUDIO    1
#define CLASS_CDCCNTL  2
#define CLASS_HID      3
#define CLASS_PHYSICAL 5
#define CLASS_IMAGE    6
#define CLASS_PRINTER  7
#define CLASS_MASS     8
#define CLASS_HUB      9
#define CLASS_CDCDATA  10
#define CLASS_CHIPCRD  11
#define CLASS_CONTENT  13
#define CLASS_WIRELESS 224


#define SUBCLASS_COMM_DLCM   1	// Direct line control model
#define SUBCLASS_COMM_ACM    2	// Abstract control model
#define SUBCLASS_COMM_TCM    3	// telephone control model
#define SUBCLASS_COMM_MCCM   4	// Multi-channel control model
#define SUBCLASS_COMM_CAPICM 5	// CAPI control model
#define SUBCLASS_COMM_ENNCM  6	// Ethernet networking control model
#define SUBCLASS_COMM_ATMNCM 7	// ATM networking control model

#define SUBCLASS_HID_BOOT 1

#define PROTOCOL_HID_KEYBRD 1
#define PROTOCOL_HID_MOUSE  2

#define SUBCLASS_MASS_RBC    1
#define SUBCLASS_MASS_ATAPI  2
#define SUBCLASS_MASS_QIC157 3
#define SUBCLASS_MASS_UFI    4
#define SUBCLASS_MASS_SFF    5
#define SUBCLASS_MASS_SCSI   6


// Define setup request type values

#define SETUP_INPUT     0x80
#define SETUP_OUTPUT    0x00
#define SETUP_DEVICE    0x00
#define SETUP_INTERFACE 0x01
#define SETUP_ENDPOINT  0x02
#define SETUP_OTHER     0x03
#define SETUP_STANDARD  0x00
#define SETUP_CLASS     0x20
#define SETUP_VENDOR    0x40

// Define the setup request values

#define TYPE_GET_STATUS    0	// Standard/all
#define TYPE_CLR_FEATURE   1	// Standard/all
#define TYPE_GET_REPORT    1	// Interface - HID class specific
#define TYPE_GET_IDLE      2	// Interface - HID class specific
#define TYPE_SET_FEATURE   3	// Standard/all
#define TYPE_GET_PROTOCOL  3	// Interface - HID class specific
#define TYPE_SET_ADDRESS   5	// Standard/device
#define TYPE_GET_DESC      6	// Standard/device
#define TYPE_SET_DESC      7	// Standard/device
#define TYPE_GET_CONFIG    8	// Standard/all
#define TYPE_SET_CONFIG    9	// Device - standard
#define TYPE_SET_REPORT    9	// Class/interface - HID class specific
#define TYPE_SET_IDLE      10	// Class/interface - HID class specific
#define TYPE_GET_INTERFACE 10	// Standard/Interface
#define TYPE_SET_INTERFACE 11	// Standard/interface
#define TYPE_SET_PROTOCOL  11	// Class/interface - HID class specific
#define TYPE_SYNCH_FRAME   12	// Standard/endpoint

// Define descriptor types

#define DESC_DEVICE    1
#define DESC_CONFIG    2
#define DESC_STRING    3
#define DESC_INTERFACE 4
#define DESC_ENDPOINT  5
#define DESC_DEVQUAL   6
#define DESC_OTHERSPD  7
#define DESC_IFPOWER   8
#define DESC_HID       33
#define DESC_REPORT    34
#define DESC_PHYSICAL  35

// Define the device descriptor

typedef _Packed struct
{	uchar  length;				// Descriptor length
	uchar  desctype;			// Descriptor type (1)
	ushort usbspec;				// USB version (BCD)
	uchar  class;				// Class
	uchar  subclass;			// Sub-class
	uchar  protocol;			// Protocol
	uchar  maxpktsz0;			// Maximum packet size for end-point 0
	ushort vendorid;			// Vendor ID
	ushort deviceid;			// Device ID
	ushort devrel;				// Device release
	uchar  manufacturer;		// Index for manufacturer string
	uchar  product;				// Index for product string
	uchar  serialnum;			// Index for serial number string
	uchar  numcfgs;				// Number of configurations
} DEVDESC;

// Define the configuration descriptor

typedef _Packed struct
{	uchar  length;				// Descriptor length
	uchar  desctype;			// Descriptor type (2)
	ushort ttllen;				// Total length of all configuration descriptors
	uchar  numifs;				// Number of interfaces
	uchar  cfgvalue;			// Configuration selectin value
	uchar  descindex;			// Index for description string
	uchar  attrib;				// Attributes
	uchar  maxpower;			// Maximum power taken from bus
} CFGDESC;

// Define the interface descriptor

typedef _Packed struct
{	uchar  length;				// Descriptor length
	uchar  desctype;			// Descriptor type (4)
	uchar  number;				// Interface number
	uchar  altsel;				// Alternate selection value
	uchar  numept;				// Number of end-points
	uchar  class;				// Class
	uchar  subclass;			// Sub-class
	uchar  protocol;			// Protocol
	uchar  descindex;			// Index for description string
} IFDESC;

// Define the end-point descriptor

typedef _Packed struct
{	uchar  length;				// Descriptor length
	uchar  desctype;			// Descriptor type (5)
	uchar  number;				// End-point number
	uchar  attrib;				// Attributes
	ushort maxpktsz;			// Maximum packet size
	uchar  pktrate;				// Packet rate (interval)
} EPDESC;

// Define the HID class descriptor

typedef _Packed struct			// Additional descriptor list element
{	uchar  type;				// Descriptor type
	ushort length;				// Descriptor length
} HIDLIST;

typedef _Packed struct
{	uchar   length;				// Descriptor length
	uchar   desctype;			// Descriptor type (0x21)
	ushort  hidver;				// HID version
	uchar   country;			// Country code
	uchar   numdesc;			// Number of additional descriptors
	HIDLIST desclist[1];		// Additional descriptor list
} HIDDESC;

// Define the hub class descriptor

typedef _Packed struct
{	uchar  length;				// Descriptor length
	uchar  desctype;			// Descriptor type (0x29)
	uchar  numports;			// Number of ports
	ushort attrib;				// Hub characteristics
	uchar  on2good;				// Time from power on to power good on a port
	uchar  contrcurrent;		// Maximum current for hub controller
	uchar  data[16];			// Data (devicermv and portpwrmask variable
								//   length values)
} HUBDESC;

// Define USB hub port status bits (high order 16 bits are only used internally
//   by USBCTL

#define USBPS_DBN 0x80000000	// Port is being debounced (root ports only)
#define USBPS_RST 0x40000000	// Port is being reset (root ports only)
#define USBPS_PIC 0x00001000	// Port indicator control
#define USBPS_PTM 0x00000800	// Port test mode
#define USBPS_HSD 0x00000400	// High speed device attached
#define USBPS_LSD 0x00000200	// Low speed device attached
#define USBPS_PPS 0x00000100	// Port power status
#define USBPS_PRS 0x00000010	// Port reset status
#define USBPS_OCI 0x00000008	// Over current indication
#define USBPS_PSS 0x00000004	// Port suspended status
#define USBPS_PES 0x00000002	// Port enabled status
#define USBPS_PCS 0x00000001	// Port connected status
