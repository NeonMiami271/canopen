#include <DS401_header.h>

#if CHECK_VERSION_APPL(2, 1, 0)
//  CHECK_VERSION_APPL(2, 0, 0)
//  CHECK_VERSION_APPL(1, 2, 0)

int32 sample_data[4];	// ������ ������. ��������� � �������������� �������� �����������.
						// ������������ � �������� ��������� ������ ��� ���������� ������� CANopen.

const unsigned8 array_size=4;	// ����� ����������� CANopen �������. ��� ������� ������� ����� ������� ������� ������.

// ������� ����������� CANopen ��������.
// ��������� ���������� ��������� ������� CANopen �� ������ ������������ ���������� ������.
static void get_object_standev(struct object *obj)
{
	obj->size = CAN_ERRET_OBD_NOSUBIND;
	obj->deftype = 0;
	obj->access = 0;
	obj->pnt = NULL;

	if (obj->index == 0x6100) {		// ������ ���������� �������, ��� �������� ������������ ����������� ���������� ��������
	  								// � ��������� (0x6000..0x9FFF) 
	                                // ��� ���������� ������������� ���������� � ��������� (0x2000..0x5FFF)
	                                // ������� � ��������� (0x2000..0x5FFF) ������������� ���������� � ����� DS401_manspec.c
		if (obj->subind == 0) {						// ��������� ���������� �������
			obj->size = sizeof(unsigned8);					// ������ ������� � ������
			obj->deftype = CAN_DEFTYPE_UNSIGNED8;			// ��� ������ �������
			obj->access = CAN_MASK_ACCESS_RO;			 	// ������ ������ �� ������
			obj->pnt = (canbyte*)&array_size;  				// �������� ��������� �� ����� �����������.
		} else if (obj->subind <= 4) {				// ���������� ���������� �������
			obj->size = sizeof(int32);						// ������ ������� � ������
			obj->deftype = CAN_DEFTYPE_INTEGER32;			// ��� ������ �������
			obj->access = CAN_MASK_ACCESS_RO | CAN_MASK_ACCESS_PDO;		// ������ ������ �� ������ � ����������� PDO �����������
			obj->pnt = (canbyte*)&sample_data[obj->subind-1];			// �������� ��������� �� ������ - �������� ���������� ������
		}
		return;
	}

	obj->size = CAN_ERRET_OBD_NOOBJECT;
}

// ������� ������� ������� CANopen �������
// ����������� ������� � ��������� �������� 0x6000..0x9FFF
int32 server_get_standev_objsize(canindex index, cansubind subind)
{
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	return obj.size;
}

// ������� ������� ���� ������� � CANopen �������
// ����������� ������� � ��������� �������� 0x6000..0x9FFF
int16 server_see_standev_access(canindex index, cansubind subind)
{
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	if (obj.size <= 0) return (int16)obj.size;
	return obj.access;
}

// ������� ������� ���� CANopen �������
// ����������� ������� � ��������� �������� 0x6000..0x9FFF
int16 server_get_standev_objtype(canindex index, cansubind subind)
{
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	if (obj.size <= 0) return (int16)obj.size;
	return obj.deftype;
}

// ������� ����������� ������ CANopen ������� �� ���������� �������
// ����������� ������� � ��������� �������� 0x6000..0x9FFF
int16 server_read_standev_objdict(canindex index, cansubind subind, canbyte *data)
{
	int16 size, cnt;
	canbyte *bpnt;
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	if (obj.size <= 0) return (int16)obj.size;
	if ((obj.access & CAN_MASK_ACCESS_RO) == 0) return CAN_ERRET_OBD_WRITEONLY;
	size = obj.size;
	bpnt = obj.pnt;
	OBJECT_DICTIONARY_TO_CANOPEN
	return CAN_RETOK;
}

// ������� ���������� ������ CANopen ������� � ��������� �������
// ����������� ������� � ��������� �������� 0x6000..0x9FFF
int16 server_write_standev_objdict(canindex index, cansubind subind, canbyte *data)
{
	int16 size, cnt;
	canbyte *bpnt;
	struct object obj;

	obj.index = index;
	obj.subind = subind;
	get_object_standev(&obj);
	if (obj.size <= 0) return (int16)obj.size;
	if ((obj.access & CAN_MASK_ACCESS_WO) == 0) return CAN_ERRET_OBD_READONLY;
	size = obj.size;
	bpnt = obj.pnt;
	FROM_CANOPEN_TO_OBJECT_DICTIONARY
	return CAN_RETOK;
}

void enter_error_state(void)
{
}

void exit_error_state(void)
{
}

void start_hardware(void)
{
}

void stop_hardware(void)
{
}

void application_timer_routine(void)
{
	// ���������� �� CANopen �������
	// ����� ����� ������ �������� ������� ��� �������.
  
//	transmit_can_pdo(0x1800);	// ������ �������� TPDO � ����� �������� 254 ��� 255
								// � �������� ��������� ������������ ������ ����������������� ������� TPDO
}

void application_monitor_routine(void)
{
	// ���������� �� CANopen �������� (�������� ����� ���������)
	// ����� ����� ������ �������� ������� ��� �������.
}

void slave_init_standev_application(void)
{
	unsigned8 chan;

	for (chan = 0; chan < 4; chan++) {
		sample_data[chan] = 0;
	}
	// ����� ����� (����)���������������� ���������� ������, ������������ � �������� CANopen ��������.
}

void slave_init_standev_communication(void)
{
  
	// ��������� PDO ����������� ���� TPDO ��� CANopen ��������
  
	server_write_obd_u32(0x1A00, 0, 0);				// ����������� ������� TPDO (PDO invalid)
	server_write_obd_u32(0x1A00, 1, 0x61000120);	// ����������� ������� ���������� (������� ������� �������) � ������ TPDO
	server_write_obd_u32(0x1A00, 2, 0x61000220);	// ����������� ������� ���������� (������ ������� �������) � ������ TPDO
	server_write_obd_u32(0x1A00, 0, 2);				// ������ ����� ��������, ������������ � ������ TPDO (���)


	server_write_obd_u32(0x1A01, 0, 0);				// ����������� ������� TPDO (PDO invalid)
	server_write_obd_u32(0x1A01, 1, 0x61000320);	// ����������� �������� ���������� (������ ������� �������) �� ������ TPDO
	server_write_obd_u32(0x1A01, 2, 0x61000420);	// ����������� ���������� ���������� (������ ������� �������) �� ������ TPDO
	server_write_obd_u32(0x1A01, 0, 2);				// ������ ����� ��������, ������������ �� ������ TPDO (���)
	
	server_write_obd_u32(0x1800, 0x1, 0x80000184);	// ����-��������� COB-ID ������� TPDO �� �������� 0x184 � �� �������������� ��������� (PDO invalid)
													// ������ ��� ������������� ����-��������� ����������������� ������������� CANopen ���������������
													// ����� ����� ����� ���� ������������ �������������� �������� PDO COB-ID 0x00000184 (PDO valid)

	server_write_obd_u32(0x1801, 0x1, 0x80000194);	// ����-��������� COB-ID ������� TPDO �� �������� 0x194 � �� �������������� ��������� (PDO invalid)
													// ������ ��� ������������� ����-��������� ����������������� ������������� CANopen ���������������
													// ����� ����� ����� ���� ������������ �������������� �������� PDO COB-ID 0x00000194 (PDO valid)

	set_pdo_state(0x1800, VALID);					// ��������� ������� TPDO (PDO valid)
	set_pdo_state(0x1801, VALID);					// ��������� ������� TPDO (PDO valid)

}

void application_init_device_routine(void)
{
}

#endif
