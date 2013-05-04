-- MySQL dump 10.11
--
-- Host: localhost    Database: smth
-- ------------------------------------------------------
-- Server version	5.0.45-community-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES latin1 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `help`
--

DROP TABLE IF EXISTS `help`;
CREATE TABLE `help` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `modeid` int(10) unsigned default '0',
  `prekey` varchar(10) NOT NULL default '',
  `func` varchar(40) NOT NULL default '',
  `content` text,
  PRIMARY KEY  (`id`),
  KEY `modeid` (`modeid`)
) ENGINE=MyISAM AUTO_INCREMENT=298 DEFAULT CHARSET=latin1 COMMENT='newhelp';

--
-- Dumping data for table `help`
--

LOCK TABLES `help` WRITE;
/*!40000 ALTER TABLE `help` DISABLE KEYS */;
INSERT INTO `help` VALUES (1,0,'<Welcome>','[1;4;31m����ϵͳ���[m','\n\n    ����ϵͳ�Ƿ�ģʽ�ģ�һ��ֻ��ʾ��ǰģʽ�µİ�����\n\n    ��s�����ڵ�ǰģʽ�µİ�����������������Χ�����򵥹��ܺ;���������\n\n    ���˰ɣ� ok, ף����ˮľ��;���'),(2,3,'k','����һƪ����',NULL),(3,3,'P','����һҳ',NULL),(4,3,'j','����һƪ����',NULL),(5,3,'N','����һҳ',NULL),(96,3,'Ctrl+G','�ض�ģʽ�Ķ�','�л�ģʽ��: 1)��ժ 2)ͬ���� 3)��m���� 4)ԭ�� 5)ͬ���� 6)����ؼ���\n            7)��������ѡ�� 8)���澫�������� 9)��ɾ����'),(8,3,'x','���뾫����',NULL),(9,3,'l','�鿴ѶϢ��¼',''),(10,3,'!','������վ',''),(11,3,'u','��ѯ������Ϣ',NULL),(13,3,'Home','������һƪ����',NULL),(14,3,'End','�������һƪ����',NULL),(15,3,'Ctrl+A','�鿴���߼��',NULL),(16,3,'<Tab>','�鿴���滭��','�鿴��ǰ����Ľ��滭�棨����¼��'),(17,3,'Ctrl+L','��Ļ�ػ�','Telnet��½��������ʱʹ��'),(19,3,'Ctrl+Z','��ѶϢ��鿴��ʷѶϢ',NULL),(200,8,'Ctrl+t','�л���ʱ�ǳƲ�ɫ��ʾ',''),(23,3,'## <cr>','���� ## ������','ֱ���������֣����س�����������ƪ���¡�'),(31,3,'p','��ͬ�����Ķ�',NULL),(32,3,'Ctrl+U','��ͬ�����Ķ�',NULL),(34,3,'= \\','��ͬ�����һƪ/���һƪ����',''),(201,14,'s','��ʾ/�����ż���С',''),(36,3,'[ ]','�ƶ�����ͬ������/��һƪ����',NULL),(37,3,'A a','��ǰ/��Ѱ����ͬ���ߵ�����',NULL),(38,3,'? /','��ǰ/����������ؼ���',NULL),(40,3,'s','ѡ��������','s������#���س������԰��չؼ��ʲ�����ذ��档'),(41,3,'Ctrl+P','����һƪ����',NULL),(42,3,'E','�޸��ѷ������µ����ݻ򸽼�','��ͨ�û�ֻ���޸��Լ�������'),(43,3,'T','�޸��ѷ������µı���','��ͨ�û�ֻ���޸��Լ�������'),(44,3,'d','ɾ���ѷ�������','��ͨ�û�ֻ��ɾ���Լ�������'),(45,3,'Ctrl+C','ת�����µ�����������',NULL),(46,3,'Ctrl+R','���Ÿ���������',NULL),(47,3,'F','ת������','����id�Ļ�վ������\r\n����Email��ַ����Internet�ʼ�'),(49,3,'i','[1;31m[����][m �����·����ݴ浵','�����ھ��������԰�i�����ݴ浵'),(50,3,'I','[1;31m[����][m ������ֱ�ӷ��뾫����','֮ǰҪ����˿·'),(51,3,'g','[1;31m[����][m �����·�����ժ��',''),(53,3,'W','[1;31m[����][m �༭����¼��ģ�塢���',''),(55,3,'D','[1;31m[����][m ����ɾ������',''),(56,3,'m','[1;31m[����][m �������Ϊm���ƶ��õ�','�ڰ�������ǰ��m����Ǵ�ƪ����Ϊm������ȥ��m��ǡ�\n���õ�ǰ���������õ�˳��'),(92,3,'Ctrl+D','[1;31m[����][m ȡ���û�����Ȩ��',''),(91,3,'b','[1;31m[����][m ִ������������⹦��',''),(59,3,'Y','�ָ���ɾ������','�����ɻָ�����վ���£��û��ɻָ���ɾ�����¡�'),(97,3,'.','[1;31m[����][m ����ɾ����',''),(65,3,'v','�鿴�����ż�','���ǲ鿴���е��ż���'),(63,3,'t','[1;31m[����][m ���ɾ��','D 0 0 y����ɾ�����б��ɾ�������£�����������'),(77,2,'a','���Ӱ���',''),(68,8,'c','�޸���ʱ�ǳ�','�������: �޸���ʱ�ǳƣ�����ʹ����ɫ���ƴ��룬����һ��Esc������'),(70,8,'M��m','���Ÿ�����','�� m �����ɼ��Ÿ������б��еĺ���'),(71,8,'S��s','��ѶϢ','�����ѷ���ѶϢ'),(79,2,'d','ɾ�������Ŀ¼',''),(76,2,'A','����Ŀ¼',''),(78,2,'m','�ƶ������Ŀ¼',''),(80,4,'a','[1;31m[����][m �����ļ�',''),(82,4,'g','[1;31m[����][m ����Ŀ¼',''),(83,4,'d','[1;31m[����][m ɾ���ļ���Ŀ¼',''),(84,5,'Ctrl+W','����',''),(85,4,'m','[1;31m[����][m �ƶ��ļ���Ŀ¼',''),(86,5,'Ctrl+Y','ɾ����ǰ��',''),(87,8,'K��k','�߳�վ��','���Լ���idǰ��k����K�����԰��Լ��Ĵ����߳�վ��'),(88,8,'D��d','ɾ������','�Ӻ����б���ɾ��һ������'),(89,8,'F��f','�л�ʹ����/����ģʽ','����F����f����ʹ����ģʽ�ͺ���ģʽ֮������л�'),(90,8,'W��w','�л������ǳ�/˵��ģʽ','����W����w���ں����ǳ�ģʽ��˵��ģʽ֮������л�'),(93,8,'Ctrl+Z','�鿴���߻ظ���Ϣ','����Ctrl��Z�������Բ鿴��Ϣ���߻ظ�'),(94,8,'/','��ID���Һ���','�����û�ID���س��������ߺ����б��в��Ҳ�������ƶ�����IDǰ'),(95,8,'e��q','�뿪�����б�','�뿪�����б���������'),(98,8,'p����','��һ�����ѻ�ʹ����','����p����������һ�����ѻ�ʹ����'),(99,5,'Ctrl+Z','�쿴��Ϣ','�쿴��һ����Ϣ'),(100,8,'n��j����','��һ�����ѻ�ʹ����','����n��j����������һ�����ѻ���ʹ����'),(101,5,'Ctrl+X','����','�������£�����ͬctrl w'),(102,5,'Ctrl+V','���ֵ�','����������뵥��'),(103,5,'Ctrl+B','��һҳ','����ƶ�����һҳ'),(104,8,'O��o','���Ӻ���','���Ӻ���'),(106,8,'P,^B,b','��һҳʹ����','����P��Ctrl+B��b���鿴��һҳʹ����'),(107,8,'N,^F,Spc','��һҳʹ����','����N��Ctrl��F���ո���鿴��һҳʹ����'),(108,8,'PageUp','��һҳʹ����','����PageUp���鿴��һҳʹ����'),(110,8,'PageDown','��һҳʹ����','����PageDown�鿴��һҳʹ����'),(263,5,'Esc+b','�趨����ɫ��','�Ȱ�Esc���ٰ�b\n�����趨����ɫ��'),(112,5,'Ctrl+Q','�༭����ָ��˵��','�����༭����ָ��˵��'),(113,8,'$','�������һ��ʹ����','��$��shift��4�����������һ��ʹ���߻����'),(114,8,'## <cr>','������ ## ��ʹ����','��������Ȼ��س�ֱ�������� ## ��ʹ����'),(115,5,'Ctrl+G','��ת','��ת���ڼ���'),(118,5,'Ctrl+O','�����ַ�����','�ɵ����������빤�ߣ������¼��ࣺ\n1.�Ӽ��˳�\n2.һ������\n3.���£ã�\n4.����б��'),(279,5,'Esc+z','�������� (��������״̬)',''),(120,5,'��','����','��������ƶ�'),(122,5,'��','����','��������ƶ�'),(123,2,'S','����������/������ģʽ�л�','����S���ڶ���������Ͳ�����ģʽ֮������л�'),(124,5,'��','����','��������ƶ�'),(125,2,'s','ѡ��������','����s��ѡ����������֮��#�������������͹ؼ���'),(126,5,'��','����','��������ƶ�'),(127,2,'T','��Ŀ¼����','����T����Ŀ¼����'),(173,3,'H','�鿴������Ϣ���ռǼ��ΰ淽��','ѡ���Ķ�: 1)ʮ������ 2)ʮ��ף�� 3)�����ȵ� 4)ϵͳ�ȵ� 5)�����ռ� 6)�ΰ淽��'),(129,2,'O��o','�鿴���ߺ�������','�鿴���ߺ�������'),(130,2,'v','�鿴�ż�','��������鿴�ż�'),(131,2,'L��l','�鿴�����¼','�鿴�����¼'),(132,2,'W��w','������Ϣ�������û�','������Ϣ�������û�'),(133,2,'Ctrl+Z','�鿴���߻���Ϣ','�鿴���߻���Ϣ'),(134,2,'Home','�ص���һ��������','�ص���һ��������'),(135,2,'End��$','�ص����һ��������','�ص����һ��������'),(136,2,'k����','��һ��������','��һ��������'),(137,2,'j����','��һ��������','��һ��������'),(138,6,'/h','�鿴������Ϣ','����/h���س������Եõ�������Ϣ���'),(139,2,'C��c','�л�������Ŀ/�������ģʽ','����C����c����������Ŀģʽ�Ͱ������ģʽ֮������л�'),(140,6,'/b','�˳�',''),(142,7,'D��d','[1;31m[����][m ɾ��ͶƱ','ֱ��ɾ��ͶƱ'),(143,5,'Ctrl+A','������','����ƶ�������'),(144,5,'Home','������','����ƶ�������'),(145,5,'Ctrl+E','����ĩ','����ƶ�����ĩ'),(146,5,'End','����ĩ','����ƶ�����ĩ'),(147,5,'Ctrl+F','��һҳ','����ƶ�����һҳ'),(148,5,'PgUp','��һҳ','����ƶ�����һҳ'),(149,5,'PgDn','��һҳ','����ƶ�����һҳ'),(150,5,'Ctrl+S','�ļ���ͷ','����ƶ����ļ���ͷ'),(151,5,'Ctrl+T','�ļ���β','����ƶ����ļ���β'),(152,5,'Esc+s','��Ѱ','��Ѱ�ִ�'),(153,5,'Esc+n','������Ѱ','������ѰEsc+s�趨���ִ�'),(154,5,'Esc+g','��ת','��ת���ڼ���'),(155,5,'Ctrl+N','��ʾ���','�л���ʾ/����ʾ��ǣ���ʾʱ״̬�����б��~������ʾʱ״̬����ģʽ��Ϊ��'),(156,5,'Ctrl+R','�л�����','�л�����/����ģʽ'),(157,5,'Ctrl+P','�л��Զ�����','�л��Զ����У����Զ�����ʱ״̬������ʾX���Զ�����ʱ״̬����ģʽ��Ϊ��'),(158,5,'Ins','�л�����/�û�ģʽ','�л�����/�û�ģʽ��״̬������ʾ��ǰģʽ'),(159,0,'Ctrl+V','���ֵ�','\n\n                            �ܺ��õ�Ŷ'),(160,4,'F','ת�ĵ�����',''),(161,3,'Ctrl+B','��һҳ',''),(162,3,'Ctrl+F','��һҳ',''),(163,3,'c','�������ǰ���µ�δ�����',''),(164,10,'a','����','\n��ģ��Ŀ¼����һ��ģ��\n������Ŀ¼����һ������'),(165,10,'d','ɾ��','\n��ģ��Ŀ¼ɾ����ǰ��ģ��\n������Ŀ¼ɾ����ǰ������'),(166,10,'t','����',''),(167,10,'x','�鿴����',''),(168,10,'s','�鿴����',''),(169,10,'i','�޸ı���',''),(170,10,'f','�޸�����',''),(171,10,'<ģ�����>','�����������','\n  һ��ģ���� ģ�����ƣ����⣬�������� ���\n\n  ģ��������ģ���г���ʱ��ʾ�����ģ�Ϊ����ʽ���ָ���ģ�塣\n  ģ������Ƿ��ĺ�ı��⣬�������������\n  �����Ƿ��ĺ���������ݣ��������������\n\n  �ڱ���������������� [$n] ��ʾ���õ�N������Ĵ�\n  ע��������������˵����ǿյĻ��������������½����ǿ�����\n\n  ����ģ�����Ϊ [$1]Ҫ����[$2]\n  ģ������Ϊ ʱ��:[$3]\n  ��һ�������һش���\"KCN\",��2�������һش���\"��\",��3�������һش���\"��������\"\n  ��ô�ᷢ��һƪ���±�����\"KCNҪ������\",������\"ʱ��:��������\"'),(172,2,'H','�鿴������Ϣ','ѡ���Ķ�: 1)����ʮ������ 2)ף����У�� 3)�����ȵ� 4)ϵͳ�ȵ� 5)�����ռ�[1'),(174,5,'Ctrl+H','ɾ�����ַ�','ɾ������󷽵��ַ�'),(175,5,'Backspace','ɾ�����ַ�','ɾ��������ַ�'),(176,5,'Ctrl+D','ɾ���ַ�','ɾ����괦�ַ�'),(177,5,'Del','ɾ���ַ�','ɾ����괦�ַ�'),(178,5,'Ctrl+K','ɾ����ĩ','�ӹ�괦ɾ������ĩ'),(179,5,'Ctrl+U','����궨','����궨��ǰ��'),(180,5,'Ctrl+C','��������','����ʹ��Ctrl+U�궨������'),(181,5,'Esc+d','�������','���Ctrl+U�궨������'),(182,5,'Esc+q','����궨','���Ctrl+U�ı궨'),(183,5,'Esc+i','��ȡ������','��ȡ�����������ݣ���ѡ��0-7ҳ'),(184,5,'Esc+e','д�������','д�����������д��0-7ҳ'),(185,5,'Esc+c','��ʾ��ɫ�༭�ɹ�','��ʾ��ɫ�༭�ɹ�����������ɷ��ر༭����'),(258,3,',','�л�������ʾ',''),(187,14,'d','ɾ���ż�','ɾ��ѡ�е��ż�'),(188,6,'/r','�鿴��ǰ������',''),(189,6,'/j room','������Ӧ������',''),(234,4,'c','[1;31m[����][m �����ļ���Ŀ¼',''),(235,4,'x','[1;31m[����][m �����ļ���Ŀ¼',''),(191,4,'s','[1;31m[����][m �л�����/��������',''),(192,3,'f','�������δ�����',''),(193,10,'b','�趨Ϊ��������',''),(194,3,'y','����blog','����ǰ�����������Լ���blog��'),(197,3,'V','�μ�ͶƱ',''),(198,3,';','[1;31m[����][m ����re���Ƽ����»��ö�','��;�������ʾ���в�������'),(199,3,'CTRL+O','�����û�������������',''),(202,14,'M','�ƶ��ż�','�����ĸ�ϵͳ������Զ�������֮���ƶ��ż�'),(203,14,'Ctrl+C','ת���ż�','��ѡ�е��ż�ת�����ض���������ע��ÿһ���ż�ֻ��ת��һ��'),(204,14,'F','ת���ż�','��ѡ�е��ż�ת�ĸ��ض��û�'),(205,14,'m','����ż�','���ѡ�е��ż���������ɾ���ż�ʱ��ѡ��ʹ����ɾ��'),(206,14,'i','[1;31m[����][m ���ż������ݴ浵','��ѡ�е��ż��������ĸ����ݴ浵'),(207,14,'D','����ɾ��','����ɾ���ż�����������ɾ���������5�������ɹ�ѡ��'),(208,14,'Ctrl+A','�鿴����������','�鿴�����˸�������'),(209,14,'o','�鿴���ߺ���','�鿴���ߺ�������'),(214,0,'F9','����','������Ļ���ݣ����ŵ��Լ����䣬��Ȼ��ǰ�������з���Ȩ�ޣ���������û����Ŷ'),(211,3,'Ctrl+Q','�鿴�����Ķ� url ���Լ������༭ url��','�鿴���µ�url����web�µĵ�ַ��\n�������������и����༭��Ȩ�ޣ���ͬʱ��ʾ�����༭�� url ��ַ��'),(213,3,'^','��ת���������ظ�������',''),(216,3,'z','����ǰ�������߷���Ϣ','���������߷���Ϣ������������߲����ߣ�����ʾ�����û�������Ϣ'),(217,3,'w','��ѶϢ',''),(218,0,'F10','�ռ�',''),(219,3,'r','�ظ�����',''),(220,3,'o','�����Ϻ���','����ͨ�����˵�-����ѡ�������趨����������'),(221,14,'? /','��ǰ/����������ؼ���',''),(222,14,'## <cr>','���� ## ���ż�',''),(223,14,'A a','��ǰ/��Ѱ��ͬһ���ߵ��ż�',''),(224,14,'Ctrl+B','��һҳ',''),(225,14,'Ctrl+F','��һҳ',''),(226,14,'Ctrl+L','��Ļ�ػ�','Telnet��¼��������ʱʹ��'),(227,14,'E','�޸��ż�����',''),(229,14,'j','����һ����',''),(230,14,'k','����һ����',''),(231,14,'r','�ظ��ż�',''),(232,14,'T','�޸��ż�����',''),(233,14,'z','���ż����߷���Ϣ',''),(236,4,'p','[1;31m[����][m ճ���ļ���Ŀ¼',''),(237,4,'f','[1;31m[����][m �趨˿·',''),(238,4,'i','[1;31m[����][m ���ݴ浵���·��뾫����',''),(239,4,'t','[1;31m[����][m �޸��ļ���Ŀ¼����',''),(240,4,'n','[1;31m[����][m �޸��ļ���Ŀ¼��Ӣ����',''),(241,4,'Ctrl+C','ת�����µ�����',''),(242,4,'Ctrl+L','��Ļ�ػ�','Telnet��½��������ʱʹ��'),(243,4,'e','[1;31m[����][m �༭�ļ�����',''),(244,1,'z','����/�����İ���','z��һ������󣬸ð��治�ٳ����ڰ����б��\n���԰�y��ʾ���а��档'),(245,1,'y','��ʾ/����ʾ���а���',''),(246,1,'a','�����������˶�����',''),(247,1,'Ctrl+A','�鿴��������',''),(248,1,'S','��������/�������л�','������ĸ˳������'),(284,2,'F9','��Ļ����','��F9��֮��ϵͳ�Զ�������ǰ��Ļ�����͵�վ������'),(250,3,'F10','�ռ�����','��Esc�˳��ռ�����'),(251,3,'Ctrl+V','���ֵ�',''),(252,6,'/w','�鿴���������û�',''),(253,6,'/e','�뿪����㳡',''),(254,6,'/q','��ѯ����',''),(264,5,'Esc+f','�趨ǰ��ɫ��','�Ȱ�Esc���ٰ�b\n�����趨ǰ��ɫ��'),(257,4,'## <cr>','���� ## ������',''),(259,3,'#','[1;31m[����][m #���','�������ɼ�'),(260,3,'<Space>','�·�һҳ','ͬPageDown'),(261,3,'%','[1;31m[����][m %���','�������ɼ�'),(269,8,'C','�����޸��ǳ�',''),(265,5,'Esc+r','��ԭɫ��','�Ȱ�Esc���ٰ�r\n����ʹ��������ֻ�ԭΪĬ�ϵ�ɫ��'),(266,5,'Esc+Esc','���������','���ΰ���Esc����������������롣\n����ʹ�÷�������ȥASCIIart����ġ�'),(267,5,'Esc+m','ɫ�鴦��','�Ȱ�Esc���ٰ�m'),(268,5,'Esc+l','����ѶϢ��','�Ȱ�Esc���ٰ�l'),(270,3,':','�趨��������',''),(271,3,'|','[1;31m[����] [m�з�����','����ǰ���¼������з�Ϊ�µ�����'),(272,2,'U','��ѯ����','��ѯ���������'),(273,1,'U','��ѯ����','��ѯ��������'),(274,4,'U','��ѯ����','��ѯ��������'),(275,3,'U','��ѯ����','��ѯ��������'),(276,4,'w','����ѶϢ',''),(277,0,'Ctrl+Z','�鿴���߻ظ���Ϣ',''),(280,8,'x','�鿴��ǰ�û���Blog','�鿴��ǰ�û���Blog��ǰ���Ǹ��û�ӵ��Blog��'),(281,7,'O��o','[1;31m[����][m ��ǰ����ͶƱ','��ǰ����ͶƱ��ͬʱͶƱ�����������'),(282,7,'A��a','[1;31m[����][m ����һ����ͶƱ',''),(283,3,'F9','��Ļ����','��F9��֮��ϵͳ�Զ�������ǰ��Ļ�����͵�վ������'),(285,14,'F9','��Ļ����','��F9��֮��ϵͳ�Զ�������ǰ��Ļ�����͵�վ������'),(286,14,'t','�����ɾ���ż�','���ż�����/ȥ����ɾ��ǣ�X��ǣ���֮�������D-0ɾ����ɾ�ż���'),(287,14,'F10','�ռ�����','��Esc�˳��ռ�����'),(288,2,'F10','�ռ�����','��Esc�˳��ռ�����'),(289,4,'F10','�ռ�����','��Esc�˳��ռ�����'),(290,4,'F9','��Ļ����','��F9��֮��ϵͳ�Զ�������ǰ��Ļ�����͵�վ������'),(291,3,'>','��ɾ��','�鿴�Լ��ڵ�ǰ����ɾ��������'),(293,3,'&','������ͬ����ĵ�һƪ����',''),(294,8,'l','�鿴˵����','�鿴��ǰʹ���ߵ�����˵����'),(295,0,'l','�鿴˵����','�鿴��ǰ�û�������˵����'),(297,3,'Ctrl+E','[31;1;m[����][m �޸ľ��ֲ������Ա����','');
/*!40000 ALTER TABLE `help` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-04-19 13:56:19
