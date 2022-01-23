#include <lib_header.h>

#if CHECK_VERSION_CANLIB(3, 0, 1)

#define MASK_PDOMAP_INDEX		0xFFFF0000	// PDO mapping entry index mask
#define MASK_PDOMAP_SUBIND		0x0000FF00	// PDO mapping entry subindex mask
#define MASK_PDOMAP_LENGTH		0x000000FF	// PDO mapping entry object length mask

#define NOF_PDO_MAPPED	(CAN_NOF_PDO_RECEIVE+CAN_NOF_PDO_TRANSMIT)

static struct mappar {
	unsigned8 nrec;
	unsigned32 map[CAN_NOF_MAP_BYTEGR];
} pdomap[NOF_PDO_MAPPED];

static int16 find_pdo_map_entry(canindex index, int16 *entry)	// 2.2.1 some changes
{
	if (index >= CAN_INDEX_RCVPDO_MAP_MIN && index < CAN_INDEX_RCVPDO_MAP_MIN+CAN_NOF_PDO_RECEIVE) {
		*entry = index - CAN_INDEX_RCVPDO_MAP_MIN;
	} else if (index >= CAN_INDEX_TRNPDO_MAP_MIN && index < CAN_INDEX_TRNPDO_MAP_MIN+CAN_NOF_PDO_TRANSMIT) {
		*entry = index - CAN_INDEX_TRNPDO_MAP_MIN + CAN_NOF_PDO_RECEIVE;
	} else {
		*entry = -1;
		return CAN_ERRET_OBD_NOOBJECT;
	}
	if (pdomap[*entry].nrec == 0) return CAN_ERRET_PDO_MAP_DEACT;
	return CAN_RETOK;
}

static int16 pdo_map_bytes_objsize(cansubind subind)
{
	if (subind == 0) return sizeof(unsigned8);
	if (subind <= CAN_NOF_MAP_BYTEGR) return sizeof(unsigned32);
	return CAN_ERRET_OBD_NOSUBIND;
}

int16 check_pdo_map_object(canindex index)		// 2.2.1
{
	int16 entry;

	return find_pdo_map_entry(index, &entry);
}

int16 get_pdo_map_bytes_objsize(canindex index, cansubind subind)	// 2.2.1 some changes
{
	if (check_pdo_map_object(index) == CAN_ERRET_OBD_NOOBJECT) {
		return CAN_ERRET_OBD_NOOBJECT;
	}
	return pdo_map_bytes_objsize(subind);
}

int16 see_pdo_map_access(canindex index, cansubind subind)
{
	int16 size;

	size = get_pdo_map_bytes_objsize(index, subind);
	if (size <= 0) return size;
	return CAN_MASK_ACCESS_RW;
}

int16 get_pdo_map_objtype(canindex index, cansubind subind)
{
	int16 size;

	size = get_pdo_map_bytes_objsize(index, subind);
	if (size <= 0) return size;
	if (subind == 0) return CAN_DEFTYPE_UNSIGNED8;
	return CAN_DEFTYPE_UNSIGNED32;
}

int16 read_pdo_map_objdict(canindex index, cansubind subind, canbyte *data)
{
	int16 size, cnt, entry;
	canbyte *bpnt;

	find_pdo_map_entry(index, &entry);
	if (entry < 0) return CAN_ERRET_OBD_NOOBJECT;
	size = pdo_map_bytes_objsize(subind);
	if (size <= 0) return size;
	if (subind == 0) bpnt = (canbyte*)&(pdomap[entry].nrec);
	else bpnt = (canbyte*)&(pdomap[entry].map[subind-1]);
	OBJECT_DICTIONARY_TO_CANOPEN
	return CAN_RETOK;
}

static int32 test_pdo_map(canindex index, unsigned32 map)
{
	canindex mapind;
	cansubind mapsub;
	int32 size;
	int16 accs;

	mapind = (map & MASK_PDOMAP_INDEX) >> 16;
	if (mapind == 0) return CAN_ERRET_PDO_NOMAP;	// Re-store optimization for UNused objects
	mapsub = (map & MASK_PDOMAP_SUBIND) >> 8;
	accs = server_see_access(mapind, mapsub);
	if (accs < 0) return CAN_ERRET_PDO_NOMAP;
	if ((accs & CAN_MASK_ACCESS_PDO) == 0) return CAN_ERRET_PDO_NOMAP;
	if ((accs & CAN_MASK_ACCESS_RO) == 0) {
		if (index >= CAN_INDEX_TRNPDO_MAP_MIN && index <= CAN_INDEX_TRNPDO_MAP_MAX) {
			return CAN_ERRET_PDO_NOMAP;
		}
	}
	if ((accs & CAN_MASK_ACCESS_WO) == 0) {
		if (index >= CAN_INDEX_RCVPDO_MAP_MIN && index <= CAN_INDEX_RCVPDO_MAP_MAX) {
			return CAN_ERRET_PDO_NOMAP;
		}
	}
	size = CAN_SIZEOF_FACTOR * server_get_object_size(mapind, mapsub);
	if (size <= 0) return CAN_ERRET_PDO_NOMAP;
	if (size > CAN_NOF_PDOBIT_MAX || (map & MASK_PDOMAP_LENGTH) != (unsigned32)size) {
		return CAN_ERRET_PDO_ERRMAP;
	}
	return size;
}

int16 write_pdo_map_objdict(canindex index, cansubind subind, canbyte *data)	// 3.0.1 some changes
{
	int16 entry;
	int32 size;
	unsigned16 len, cnt;
	unsigned32 buf;
	canbyte *bpnt;

	find_pdo_map_entry(index, &entry);
	if (entry < 0) return CAN_ERRET_OBD_NOOBJECT;
	size = pdo_map_bytes_objsize(subind);
	if (size <= 0) return (int16)size;
	if (subind == 0) {
		if (*data == 0) {
			pdomap[entry].nrec = 0;
			return set_pdo_state((canindex)(index-0x200), NOT_VALID);
		} else {
			if (pdomap[entry].nrec != 0) return CAN_ERRET_OBD_OBJACCESS;
			if (*data > CAN_NOF_MAP_BYTEGR) return CAN_ERRET_PDO_ERRMAP;
			len = 0;
			for (cnt = 0; cnt < *data; cnt++) {
				size = test_pdo_map(index, pdomap[entry].map[cnt]);
				if (size < 0) return (int16)size;
				len += (unsigned16)size;
				if (len > CAN_NOF_PDOBIT_MAX) return CAN_ERRET_PDO_ERRMAP;
			}
			pdomap[entry].nrec = *data;
			return CAN_RETOK;
		}
	} else {
		if (pdomap[entry].nrec != 0) return CAN_ERRET_OBD_OBJACCESS;
		bpnt = (canbyte*)&buf;
		size = sizeof(unsigned32);
		FROM_CANOPEN_TO_OBJECT_DICTIONARY
		size = test_pdo_map(index, buf);
		if (size < 0) return (int16)size;
		pdomap[entry].map[subind-1] = buf;
		return CAN_RETOK;
	}
}

int16 map_pdo(canindex index, canframe *cf)
{
	int16 entry, fnr;
	unsigned16 len, cnt;
	unsigned8 nur;				// 3.0.1
	canindex mapind;
	cansubind mapsub;

	fnr = check_pdo_comm_object(index);		// 2.2.1
	if (fnr != CAN_RETOK) return fnr;
	fnr = find_pdo_map_entry((canindex)(index+0x200), &entry);
	if (fnr != CAN_RETOK) return fnr;
	cf->len = 0;
	nur = pdomap[entry].nrec;
	for (cnt = 0; cnt < nur; cnt++) {
		mapind = (pdomap[entry].map[cnt] & MASK_PDOMAP_INDEX) >> 16;
		mapsub = (pdomap[entry].map[cnt] & MASK_PDOMAP_SUBIND) >> 8;
		len = 1 + ((pdomap[entry].map[cnt] & MASK_PDOMAP_LENGTH) - 1) / CAN_SIZEOF_FACTOR;
		if ((cf->len + len) > CAN_DATALEN_MAXIMUM) return CAN_ERRET_PDO_ERRMAP;
		fnr = server_read_object_dictionary(mapind, mapsub, &cf->data[cf->len]);
		if (fnr != CAN_RETOK) return fnr;
		cf->len += (unsigned8)len;
	}
	return CAN_RETOK;
}

int16 activate_pdo(canindex index, canframe *cf)
{
	int16 entry, fnr;
	unsigned16 pdoelm, cnt;		// 2.3.1
	unsigned8 nur;				// 3.0.1
	canindex mapind;
	cansubind mapsub;

	fnr = check_pdo_comm_object(index);		// 2.2.1
	if (fnr != CAN_RETOK) return fnr;
	fnr = find_pdo_map_entry((canindex)(index+0x200), &entry);
	if (fnr != CAN_RETOK) return fnr;
	nur = pdomap[entry].nrec;
	pdoelm = 0;
	for (cnt = 0; cnt < nur; cnt++) {
		pdoelm += 1 + ((pdomap[entry].map[cnt] & MASK_PDOMAP_LENGTH) - 1) / CAN_SIZEOF_FACTOR;
		if (pdoelm > cf->len) {
			produce_emcy_default(CAN_EMCY_PDOLENERR);
			return CAN_ERRET_PDO_ERRMAP;
		}
	}
	pdoelm = 0;
	for (cnt = 0; cnt < nur; cnt++) {
		mapind = (pdomap[entry].map[cnt] & MASK_PDOMAP_INDEX) >> 16;
		mapsub = (pdomap[entry].map[cnt] & MASK_PDOMAP_SUBIND) >> 8;
		fnr = server_write_object_dictionary(mapind, mapsub, &cf->data[pdoelm]);
		if (fnr != CAN_RETOK) return fnr;
		pdo_activated_slave(mapind, mapsub);
		pdoelm += 1 + ((pdomap[entry].map[cnt] & MASK_PDOMAP_LENGTH) - 1) / CAN_SIZEOF_FACTOR;
	}
	set_pdo_recv_event_timer(index);
	return CAN_RETOK;
}

void can_init_pdo_map(void)
{
	unsigned16 entry, cnt;

	for (entry = 0; entry < NOF_PDO_MAPPED; entry++) {
		pdomap[entry].nrec = 0;
		for (cnt = 0; cnt < CAN_NOF_MAP_BYTEGR; cnt++) pdomap[entry].map[cnt] = 0;
	}
}

#endif
