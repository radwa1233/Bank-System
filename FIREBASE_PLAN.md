# خطة المرحلة الأولى — تحويل NOVA BANK لـ Firebase

> الهدف: الأساس فقط (Auth + Firestore + فصل Client/Account + Roles + نقل البيانات).
> الكروت/القروض/الإشعارات/PDF = مراحل لاحقة.

---

## الجزء أ — خطوات تعمليها إنتِ على Firebase (هرشدك خطوة بخطوة)

1. تدخلي console.firebase.google.com بحساب Google → "Add project" → اسم: `nova-bank`.
2. تفعّلي **Authentication** → Sign-in method → تفعّلي **Email/Password**.
3. تفعّلي **Firestore Database** → Create database → Start in **test mode** (مؤقتاً).
4. Project settings (⚙️) → Your apps → Web app `</>` → تنسخي كائن `firebaseConfig`.
5. تبعتيلي الـ`firebaseConfig` (مفيهوش أسرار خطيرة — آمن للمتصفح).

> ملاحظة أمان: قواعد Firestore هنضبطها صح قبل النهاية (مش test mode للأبد).

---

## الجزء ب — الكود اللي هكتبه أنا (بعد ما تديني الـconfig)

### 1. هيكل البيانات (Firestore Collections)
- `clients`  → { clientId, firstName, lastName, email, phone, nationalId, createdAt }
- `accounts` → { accountNumber, clientId, type (savings/current), balance, currency, status, createdAt }
- `transactions` → { id, fromAccount, toAccount, amount, type, date, byUser }
- `users` (الموظفين/الأدمن) → { uid, fullName, email, role (admin/employee), createdAt }
- الـRoles عبر **Custom Claims** أو حقل role في مستند المستخدم.

### 2. ملف إعداد مشترك
- `firebase-config.js` — يحتوي الـconfig + تهيئة Auth و Firestore (يُستورد في كل الصفحات).

### 3. نظام تسجيل الدخول بنوعين
- **صفحة Login موحّدة**: إيميل + باسوورد عبر Firebase Auth.
- بعد الدخول: نقرأ الـrole ونوجّه:
  - `admin` / `employee` → dashboard.html (لوحة الإدارة الحالية)
  - `customer` → صفحة جديدة `customer.html` (يشوف حساباته ومعاملاته فقط)
- **صفحة Register للعملاء**: `register.html` (إيميل + باسوورد + بيانات).
- قفل بعد محاولات + رسائل خطأ من Firebase (يغطي جزء الـSecurity).

### 4. نقل بياناتك الحالية لـ Firestore
- سكربت نقل (Python أو صفحة HTML لمرة واحدة) يقرأ `data/*.txt`
  ويكتب العملاء كـ clients + accounts، والموظفين كـ users بـ role=employee/admin.
- ملاحظة: الباسوردات القديمة (PIN) مش هتتنقل لـ Auth مباشرة — العملاء/الموظفين
  هيحتاجوا إنشاء باسوورد جديد، أو ننشئ حسابات Auth لهم ببريدهم وباسوورد مؤقت.

### 5. إعادة ربط الصفحات على Firestore (بدل سيرفر بايثون)
- clients.html / users.html / transactions.html / currency.html / dashboard.html
  تتحوّل من `fetch('/api/...')` إلى استدعاءات Firestore SDK.
- المعاملات (إيداع/سحب/تحويل) تتم كـ Firestore transaction (atomic) لضمان الرصيد.

### 6. تنظيف
- سيرفر بايثون والـtxt يبقوا للأرشيف فقط (مش مستخدمين بعد النقل).
- برنامج C++ يفضل شغّال لوحده على ملفاته (مش مرتبط بـ Firebase — غير عملي تقنياً).

---

## ترتيب التنفيذ
1. إنتِ: تعملي مشروع Firebase وتبعتي الـconfig. **(أول خطوة — موقوفين عليها)**
2. أنا: `firebase-config.js` + صفحة Login جديدة بنوعين + Register.
3. أنا: سكربت نقل بياناتك لـ Firestore.
4. أنا: صفحة customer.html للعميل.
5. أنا: إعادة ربط صفحات الأدمن على Firestore.
6. أنا: ضبط قواعد أمان Firestore.
7. اختبار شامل لكل دور (admin / employee / customer).

---

## نقاط مهمة وصريحة
- **C++ + Firebase = مش عملي** — هيفضل منفصل.
- **محتاج إنترنت** عشان يشتغل (مش offline زي دلوقتي).
- الـ8 نقاط الباقية (كروت/قروض/إشعارات/PDF/فلترة) = بعد ما الأساس يثبت ويتجرب.
